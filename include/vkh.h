/*
 * Copyright (c) 2018 Jean-Philippe Bruyère <jp_bruyere@hotmail.com>
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
#ifndef VK_HELPERS_H
#define VK_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include <vulkan/vulkan.h>

#define FB_COLOR_FORMAT VK_FORMAT_B8G8R8A8_UNORM

#define VK_CHECK_RESULT(f) 																				\
{																										\
    VkResult res = (f);																					\
    if (res != VK_SUCCESS)																				\
    {																									\
        printf("Fatal : VkResult is %d in %s at line %d\n", res,  __FILE__, __LINE__); \
        assert(res == VK_SUCCESS);																		\
    }																									\
}

typedef struct _vkh_app_t*		VkhApp;
typedef struct _vkh_phy_t*		VkhPhyInfo;
typedef struct _vkh_device_t*   VkhDevice;
typedef struct _vkh_image_t*    VkhImage;
typedef struct _vkh_buffer_t*   VkhBuffer;
typedef struct _vkh_queue_t*    VkhQueue;
//typedef struct _vkh_presenter_t*    VkhPresenter;

// VkhApp
VkhApp              vkh_app_create      (const char* app_name, int ext_count, const char* extentions[]);
void                vkh_app_destroy     (VkhApp app);
VkInstance          vkh_app_get_inst    (VkhApp app);
VkPhysicalDevice    vkh_app_select_phy  (VkhApp app, VkPhysicalDeviceType preferedPhyType);
VkhPhyInfo*             vkh_app_get_phyinfos    (VkhApp app, uint32_t* count, VkSurfaceKHR surface);
void                vkh_app_free_phyinfos   (uint32_t count, VkhPhyInfo* infos);

VkPhysicalDeviceProperties vkh_app_get_phy_properties (VkhApp app, uint32_t phyIndex);
// VkhPhy
VkhPhyInfo  vkh_phyinfo_create  (VkhApp app, VkPhysicalDevice phy, VkSurfaceKHR surface);
void        vkh_phyinfo_destroy (VkhPhyInfo phy);
// VkhDevice
VkhDevice   vkh_device_create   (VkPhysicalDevice phy, VkDevice vkDev);
void        vkh_device_destroy  (VkhDevice dev);
// VkhImage
VkhImage vkh_image_create       (VkhDevice pDev, VkFormat format, uint32_t width, uint32_t height, VkImageTiling tiling,
                                    VkMemoryPropertyFlags memprops,	VkImageUsageFlags usage);
VkhImage vkh_image_ms_create    (VkhDevice pDev, VkFormat format, VkSampleCountFlagBits num_samples, uint32_t width, uint32_t height,
                                    VkMemoryPropertyFlags memprops,	VkImageUsageFlags usage);
VkhImage vkh_tex2d_array_create (VkhDevice pDev, VkFormat format, uint32_t width, uint32_t height, uint32_t layers,
                                    VkMemoryPropertyFlags memprops, VkImageUsageFlags usage);
void vkh_image_create_descriptor(VkhImage img, VkImageViewType viewType, VkImageAspectFlags aspectFlags, VkFilter magFilter, VkFilter minFilter,
                                    VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode);
void vkh_image_create_view      (VkhImage img, VkImageViewType viewType, VkImageAspectFlags aspectFlags);
void vkh_image_create_sampler   (VkhImage img, VkFilter magFilter, VkFilter minFilter,
                                    VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode);
void vkh_image_set_layout       (VkCommandBuffer cmdBuff, VkhImage image, VkImageAspectFlags aspectMask,
                                    VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);
void vkh_image_set_layout_subres(VkCommandBuffer cmdBuff, VkhImage image, VkImageSubresourceRange subresourceRange,
                                    VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);
void vkh_image_destroy          (VkhImage img);
void* vkh_image_map             (VkhImage img);
void vkh_image_unmap            (VkhImage img);

VkImage                 vkh_image_get_vkimage   (VkhImage img);
VkImageView             vkh_image_get_view      (VkhImage img);
VkImageLayout           vkh_image_get_layout    (VkhImage img);
VkSampler               vkh_image_get_sampler   (VkhImage img);
VkDescriptorImageInfo   vkh_image_get_descriptor(VkhImage img, VkImageLayout imageLayout);
// VkhBuffer
VkhBuffer   vkh_buffer_create   (VkhDevice pDev, VkBufferUsageFlags usage,
                                    VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size);
void        vkh_buffer_destroy  (VkhBuffer buff);
VkResult    vkh_buffer_map      (VkhBuffer buff);
void        vkh_buffer_unmap    (VkhBuffer buff);
VkResult    vkh_buffer_bind     (VkhBuffer buff);

VkBuffer    vkh_buffer_get_vkbuffer         (VkhBuffer buff);
void*       vkh_buffer_get_mapped_pointer   (VkhBuffer buff);
///////////////////////////////

VkFence         vkh_fence_create			(VkDevice dev);
VkFence         vkh_fence_create_signaled	(VkDevice dev);
VkSemaphore     vkh_semaphore_create		(VkDevice dev);

VkCommandPool   vkh_cmd_pool_create (VkDevice dev, uint32_t qFamIndex, VkCommandPoolCreateFlags flags);
VkCommandBuffer vkh_cmd_buff_create (VkDevice dev, VkCommandPool cmdPool, VkCommandBufferLevel level);
void vkh_cmd_begin  (VkCommandBuffer cmdBuff, VkCommandBufferUsageFlags flags);
void vkh_cmd_end    (VkCommandBuffer cmdBuff);
void vkh_cmd_submit (VkhQueue queue, VkCommandBuffer *pCmdBuff, VkFence fence);
void vkh_cmd_submit_with_semaphores(VkhQueue queue, VkCommandBuffer *pCmdBuff, VkSemaphore waitSemaphore,
                                    VkSemaphore signalSemaphore, VkFence fence);

VkShaderModule vkh_load_module(VkDevice dev, const char* path);

bool        memory_type_from_properties(VkPhysicalDeviceMemoryProperties* memory_properties, uint32_t typeBits,
                                        VkFlags requirements_mask, uint32_t *typeIndex);
char *      read_spv(const char *filename, size_t *psize);
uint32_t*   readFile(uint32_t* length, const char* filename);

void dumpLayerExts ();

void        set_image_layout(VkCommandBuffer cmdBuff, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
                      VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);
void        set_image_layout_subres(VkCommandBuffer cmdBuff, VkImage image, VkImageSubresourceRange subresourceRange, VkImageLayout old_image_layout,
                      VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);
/////////////////////
VkhQueue    vkh_queue_create    (VkhDevice dev, uint32_t familyIndex, uint32_t qIndex, VkQueueFlags flags);
void        vkh_queue_destroy   (VkhQueue queue);
VkhQueue    vkh_queue_find      (VkhDevice dev, VkQueueFlags flags);
/////////////////////

#ifdef __cplusplus
}
#endif

#endif
