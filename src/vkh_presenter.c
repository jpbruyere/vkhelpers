/*
 * Copyright (c) 2018-2022 Jean-Philippe Bruyère <jp_bruyere@hotmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "vkh_presenter.h"
#include "vkh_device.h"
#include "vkh_image.h"

#ifndef MIN
# define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
# define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#define FENCE_TIMEOUT UINT16_MAX

void vkh_presenter_create_swapchain	 (VkhPresenter r);
void _swapchain_destroy (VkhPresenter r);
void _init_phy_surface	(VkhPresenter r, VkFormat preferedFormat, VkPresentModeKHR presentMode);

VkhPresenter vkh_presenter_create (VkhDevice dev, uint32_t presentQueueFamIdx, VkSurfaceKHR surface, uint32_t width, uint32_t height,
						   VkFormat preferedFormat, VkPresentModeKHR presentMode) {
	VkhPresenter r = (VkhPresenter)calloc(1,sizeof(vkh_presenter_t));

	r->dev = dev;
	r->qFam = presentQueueFamIdx ;
	r->surface = surface;
	r->width = width;
	r->height = height;
	vkGetDeviceQueue(r->dev->dev, r->qFam, 0, &r->queue);

	r->cmdPool			= vkh_cmd_pool_create  (r->dev, presentQueueFamIdx, 0);
	r->semaPresentEnd	= vkh_semaphore_create (r->dev);
	r->semaDrawEnd		= vkh_semaphore_create (r->dev);
	r->fenceDraw		= vkh_fence_create_signaled(r->dev);

	_init_phy_surface (r, preferedFormat, presentMode);

	vkh_presenter_create_swapchain (r);

	return r;
}

void vkh_presenter_destroy (VkhPresenter r) {
	vkDeviceWaitIdle (r->dev->dev);

	_swapchain_destroy (r);

	vkDestroySemaphore	(r->dev->dev, r->semaDrawEnd, NULL);
	vkDestroySemaphore	(r->dev->dev, r->semaPresentEnd, NULL);
	vkDestroyFence		(r->dev->dev, r->fenceDraw, NULL);
	vkDestroyCommandPool(r->dev->dev, r->cmdPool, NULL);

	free (r);
}

bool vkh_presenter_acquireNextImage (VkhPresenter r, VkFence fence, VkSemaphore semaphore) {
	// Get the index of the next available swapchain image:
	VkResult err = vkAcquireNextImageKHR
			(r->dev->dev, r->swapChain, UINT64_MAX, semaphore, fence, &r->currentScBufferIndex);
	return ((err != VK_ERROR_OUT_OF_DATE_KHR) && (err != VK_SUBOPTIMAL_KHR));
}


bool vkh_presenter_draw (VkhPresenter r) {
	if (!vkh_presenter_acquireNextImage (r, VK_NULL_HANDLE, r->semaPresentEnd)){
		vkh_presenter_create_swapchain (r);
		return false;
	}

	VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit_info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
								 .commandBufferCount = 1,
								 .signalSemaphoreCount = 1,
								 .pSignalSemaphores = &r->semaDrawEnd,
								 .waitSemaphoreCount = 1,
								 .pWaitSemaphores = &r->semaPresentEnd,
								 .pWaitDstStageMask = &dstStageMask,
								 .pCommandBuffers = &r->cmdBuffs[r->currentScBufferIndex]};

	vkWaitForFences	(r->dev->dev, 1, &r->fenceDraw, VK_TRUE, FENCE_TIMEOUT);
	vkResetFences	(r->dev->dev, 1, &r->fenceDraw);

	VK_CHECK_RESULT(vkQueueSubmit (r->queue, 1, &submit_info, r->fenceDraw));

	/* Now present the image in the window */
	VkPresentInfoKHR present = { .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
								 .swapchainCount = 1,
								 .pSwapchains = &r->swapChain,
								 .waitSemaphoreCount = 1,
								 .pWaitSemaphores = &r->semaDrawEnd,
								 .pImageIndices = &r->currentScBufferIndex };

	/* Make sure command buffer is finished before presenting */
	vkQueuePresentKHR(r->queue, &present);
	return true;
}

void vkh_presenter_build_blit_cmd (VkhPresenter r, VkImage blitSource, uint32_t width, uint32_t height){

	uint32_t w = MIN(width, r->width), h = MIN(height, r->height);

	for (uint32_t i = 0; i < r->imgCount; ++i)
	{
		VkImage bltDstImage = r->ScBuffers[i]->image;
		VkCommandBuffer cb = r->cmdBuffs[i];

		vkh_cmd_begin(cb,0);

		set_image_layout(cb, bltDstImage, VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

		set_image_layout(cb, blitSource, VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT);

		/*VkImageCopy cregion = { .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
								.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
								.srcOffset = {0},
								.dstOffset = {0,0,0},
								.extent = {MIN(w,r->width), MIN(h,r->height),1}};*/
		VkImageBlit bregion = { .srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
								.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
								.srcOffsets[0] = {0},
								.srcOffsets[1] = {w, h, 1},
								.dstOffsets[0] = {0},
								.dstOffsets[1] = {width, height, 1}
							  };

		vkCmdBlitImage(cb, blitSource, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, bltDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bregion, VK_FILTER_NEAREST);

		/*vkCmdCopyImage(cb, blitSource, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, bltDstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					   1, &cregion);*/

		set_image_layout(cb, bltDstImage, VK_IMAGE_ASPECT_COLOR_BIT,
						 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
						 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
		set_image_layout(cb, blitSource, VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

		vkh_cmd_end(cb);
	}
}
void vkh_presenter_get_size (VkhPresenter r, uint32_t* pWidth, uint32_t* pHeight){
	*pWidth = r->width;
	*pHeight = r->height;
}
void _init_phy_surface(VkhPresenter r, VkFormat preferedFormat, VkPresentModeKHR presentMode){
	uint32_t count;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR (r->dev->phy, r->surface, &count, NULL));
	assert (count>0);
	VkSurfaceFormatKHR* formats = (VkSurfaceFormatKHR*)malloc(count * sizeof(VkSurfaceFormatKHR));
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR (r->dev->phy, r->surface, &count, formats));

	for (uint32_t i=0; i<count; i++){
		if (formats[i].format == preferedFormat) {
			r->format = formats[i].format;
			r->colorSpace = formats[i].colorSpace;
			break;
		}
	}
	assert (r->format != VK_FORMAT_UNDEFINED);

	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(r->dev->phy, r->surface, &count, NULL));
	assert (count>0);
	VkPresentModeKHR* presentModes = (VkPresentModeKHR*)malloc(count * sizeof(VkPresentModeKHR));
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfacePresentModesKHR(r->dev->phy, r->surface, &count, presentModes));
	r->presentMode = -1;
	for (uint32_t i=0; i<count; i++){
		if (presentModes[i] == presentMode) {
			r->presentMode = presentModes[i];
			break;
		}
	}
	if ((int32_t)r->presentMode < 0)
		r->presentMode = presentModes[0];//take first as fallback

	free(formats);
	free(presentModes);
}

void vkh_presenter_create_swapchain (VkhPresenter r){
	// Ensure all operations on the device have been finished before destroying resources
	vkDeviceWaitIdle(r->dev->dev);

	VkSurfaceCapabilitiesKHR surfCapabilities;
	VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(r->dev->phy, r->surface, &surfCapabilities));
	assert (surfCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	// width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
	if (surfCapabilities.currentExtent.width == 0xFFFFFFFF) {
		// If the surface size is undefined, the size is set to
		// the size of the images requested
		if (r->width < surfCapabilities.minImageExtent.width)
			r->width = surfCapabilities.minImageExtent.width;
		else if (r->width > surfCapabilities.maxImageExtent.width)
			r->width = surfCapabilities.maxImageExtent.width;
		if (r->height < surfCapabilities.minImageExtent.height)
			r->height = surfCapabilities.minImageExtent.height;
		else if (r->height > surfCapabilities.maxImageExtent.height)
			r->height = surfCapabilities.maxImageExtent.height;
	} else {
		// If the surface size is defined, the swap chain size must match
		r->width = surfCapabilities.currentExtent.width;
		r->height= surfCapabilities.currentExtent.height;
	}

	VkSwapchainKHR newSwapchain;
	VkSwapchainCreateInfoKHR createInfo = { .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
											.surface = r->surface,
											.minImageCount = surfCapabilities.minImageCount,
											.imageFormat = r->format,
											.imageColorSpace = r->colorSpace,
											.imageExtent = {r->width,r->height},
											.imageArrayLayers = 1,
											.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
											.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
											.preTransform = surfCapabilities.currentTransform,
											.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
											.presentMode = r->presentMode,
											.clipped = VK_TRUE,
											.oldSwapchain = r->swapChain};

	VK_CHECK_RESULT(vkCreateSwapchainKHR (r->dev->dev, &createInfo, NULL, &newSwapchain));
	if (r->swapChain != VK_NULL_HANDLE)
		_swapchain_destroy(r);
	r->swapChain = newSwapchain;

	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(r->dev->dev, r->swapChain, &r->imgCount, NULL));
	assert (r->imgCount>0);

	VkImage* images = (VkImage*)malloc(r->imgCount * sizeof(VkImage));
	VK_CHECK_RESULT(vkGetSwapchainImagesKHR(r->dev->dev, r->swapChain, &r->imgCount, images));

	r->ScBuffers = (VkhImage*)		malloc (r->imgCount * sizeof(VkhImage));
	r->cmdBuffs = (VkCommandBuffer*)malloc (r->imgCount * sizeof(VkCommandBuffer));

	for (uint32_t i=0; i<r->imgCount; i++) {

		VkhImage sci = vkh_image_import(r->dev, images[i], r->format, r->width, r->height);
		vkh_image_create_view(sci, VK_IMAGE_VIEW_TYPE_2D, VK_IMAGE_ASPECT_COLOR_BIT);
		r->ScBuffers [i] = sci;

		r->cmdBuffs [i] = vkh_cmd_buff_create(r->dev, r->cmdPool,VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	}
	r->currentScBufferIndex = 0;
	free (images);
}
void _swapchain_destroy (VkhPresenter r){
	for (uint32_t i = 0; i < r->imgCount; i++)
	{
		vkh_image_destroy (r->ScBuffers [i]);
		vkFreeCommandBuffers (r->dev->dev, r->cmdPool, 1, &r->cmdBuffs[i]);
	}
	vkDestroySwapchainKHR (r->dev->dev, r->swapChain, NULL);
	r->swapChain = VK_NULL_HANDLE;
	free(r->ScBuffers);
	free(r->cmdBuffs);
}
