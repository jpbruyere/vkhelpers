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
#ifndef VK_HELPERS_H
#define VK_HELPERS_H

#ifdef __cplusplus
extern "C" {
#endif

#define _CRT_SECURE_NO_WARNINGS

#include <vulkan/vulkan.h>

#include "deps/tinycthread.h"

typedef enum VmaMemoryUsage VmaMemoryUsage;

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define _USE_MATH_DEFINES   //to have M_PI* defined with MSVC
#include <math.h>

#define VKH_KO      0x00000400
#define VKH_MO      0x00100000
#define VKH_GO      0x40000000

#define VK_CHECK_RESULT(f) 																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		fprintf(stderr, "Fatal : VkResult is %d in %s at line %d\n", res,  __FILE__, __LINE__); \
		assert(res == VK_SUCCESS);																		\
	}																									\
}
#ifndef vkh_public
	#ifdef VKH_SHARED_BUILD
		#if (defined(_WIN32) || defined(_WIN64))
			#define vkh_public __declspec(dllimport)
		#else
			#define vkh_public __attribute__((visibility("default")))
		#endif
	#else
		#define vkh_public 
	#endif
#endif

typedef struct _vkh_app_t*		VkhApp;
typedef struct _vkh_phy_t*		VkhPhyInfo;
typedef struct _vkh_device_t*   VkhDevice;
typedef struct _vkh_image_t*    VkhImage;
typedef struct _vkh_buffer_t*   VkhBuffer;
typedef struct _vkh_queue_t*    VkhQueue;
typedef struct _vkh_presenter_t* VkhPresenter;

/*************
 * VkhApp    *
 *************/
vkh_public
VkhApp              vkh_app_create      (uint32_t version_major, uint32_t version_minor,
										 const char* app_name, uint32_t enabledLayersCount, const char **enabledLayers, uint32_t ext_count, const char* extentions[]);
vkh_public
void                vkh_app_destroy     (VkhApp app);
vkh_public
VkInstance          vkh_app_get_inst    (VkhApp app);
//VkPhysicalDevice    vkh_app_select_phy  (VkhApp app, VkPhysicalDeviceType preferedPhyType);
vkh_public
VkhPhyInfo*         vkh_app_get_phyinfos    (VkhApp app, uint32_t* count, VkSurfaceKHR surface);
vkh_public
void                vkh_app_free_phyinfos   (uint32_t count, VkhPhyInfo* infos);
vkh_public
void                vkh_app_enable_debug_messenger (VkhApp app, VkDebugUtilsMessageTypeFlagsEXT typeFlags,
													VkDebugUtilsMessageSeverityFlagsEXT severityFlags,
													PFN_vkDebugUtilsMessengerCallbackEXT callback);

vkh_public
VkPhysicalDeviceProperties          vkh_app_get_phy_properties (VkhApp app, uint32_t phyIndex);

/*************
 * VkhPhy    *
 *************/
vkh_public
VkhPhyInfo          vkh_phyinfo_create		(VkPhysicalDevice phy, VkSurfaceKHR surface);
vkh_public
void                vkh_phyinfo_destroy		(VkhPhyInfo phy);

vkh_public
VkPhysicalDeviceProperties          vkh_phyinfo_get_properties          (VkhPhyInfo phy);
vkh_public
VkPhysicalDeviceMemoryProperties    vkh_phyinfo_get_memory_properties   (VkhPhyInfo phy);

vkh_public
void vkh_phyinfo_get_queue_fam_indices		(VkhPhyInfo phy, int* pQueue, int* gQueue, int* tQueue, int* cQueue);
vkh_public
VkQueueFamilyProperties* vkh_phyinfo_get_queues_props(VkhPhyInfo phy, uint32_t* qCount);

vkh_public
bool vkh_phyinfo_create_queues	  (VkhPhyInfo phy, int qFam, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo);
vkh_public
bool vkh_phyinfo_create_presentable_queues	(VkhPhyInfo phy, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo);
vkh_public
bool phy_info_create_graphic_queues			(VkhPhyInfo phy, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo);
vkh_public
bool vkh_phyinfo_create_transfer_queues		(VkhPhyInfo phy, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo);
vkh_public
bool vkh_phyinfo_create_compute_queues		(VkhPhyInfo phy, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo);

vkh_public
bool vkh_phyinfo_try_get_extension_properties (VkhPhyInfo phy, const char* name, const VkExtensionProperties* properties);
/*************
 * VkhDevice *
 *************/
vkh_public
VkhDevice           vkh_device_create   (VkhApp app, VkhPhyInfo phyInfo, VkDeviceCreateInfo* pDevice_info);
vkh_public
VkhDevice           vkh_device_import   (VkInstance inst, VkPhysicalDevice phy, VkDevice vkDev);
vkh_public
void                vkh_device_destroy  (VkhDevice dev);
vkh_public
void                vkh_device_init_debug_utils (VkhDevice dev);
vkh_public
VkDevice			vkh_device_get_vkdev(VkhDevice dev);
vkh_public
VkPhysicalDevice	vkh_device_get_phy	(VkhDevice dev);
/**
 * @brief Retrieve @ref VkhApp instance used to create this VkhDevice.
 * @param dev
 * @return the VkhApp instace used to create the VkhDevice.
 */
vkh_public
VkhApp	vkh_device_get_app	(VkhDevice dev);

vkh_public
void vkh_device_set_object_name (VkhDevice dev, VkObjectType objectType, uint64_t handle, const char *name);

vkh_public
VkSampler vkh_device_create_sampler (VkhDevice dev, VkFilter magFilter, VkFilter minFilter,
							   VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode);
vkh_public
void vkh_device_destroy_sampler (VkhDevice dev, VkSampler sampler);

/****************
 * VkhPresenter *
 ****************/
vkh_public
VkhPresenter vkh_presenter_create (VkhDevice dev, uint32_t presentQueueFamIdx, VkSurfaceKHR surface,
								   uint32_t width, uint32_t height,
								   VkFormat preferedFormat, VkPresentModeKHR presentMode);
vkh_public
void        vkh_presenter_destroy (VkhPresenter r);
vkh_public
bool        vkh_presenter_draw    (VkhPresenter r);
vkh_public
bool        vkh_presenter_acquireNextImage  (VkhPresenter r, VkFence fence, VkSemaphore semaphore);
vkh_public
void        vkh_presenter_build_blit_cmd    (VkhPresenter r, VkImage blitSource, uint32_t width, uint32_t height);
vkh_public
void        vkh_presenter_create_swapchain  (VkhPresenter r);
vkh_public
void		vkh_presenter_get_size			(VkhPresenter r, uint32_t* pWidth, uint32_t* pHeight);
/************
 * VkhImage *
 ************/
vkh_public
VkhImage vkh_image_import       (VkhDevice pDev, VkImage vkImg, VkFormat format, uint32_t width, uint32_t height);
vkh_public
VkhImage vkh_image_create       (VkhDevice pDev, VkFormat format, uint32_t width, uint32_t height, VkImageTiling tiling,
									VmaMemoryUsage memprops,	VkImageUsageFlags usage);
vkh_public
VkhImage vkh_image_ms_create    (VkhDevice pDev, VkFormat format, VkSampleCountFlagBits num_samples, uint32_t width, uint32_t height,
									VmaMemoryUsage memprops,	VkImageUsageFlags usage);
vkh_public
VkhImage vkh_tex2d_array_create (VkhDevice pDev, VkFormat format, uint32_t width, uint32_t height, uint32_t layers,
									VmaMemoryUsage memprops, VkImageUsageFlags usage);
vkh_public
void vkh_image_set_sampler      (VkhImage img, VkSampler sampler);
vkh_public
void vkh_image_create_descriptor(VkhImage img, VkImageViewType viewType, VkImageAspectFlags aspectFlags, VkFilter magFilter, VkFilter minFilter,
									VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode);
vkh_public
void vkh_image_create_view      (VkhImage img, VkImageViewType viewType, VkImageAspectFlags aspectFlags);
vkh_public
void vkh_image_create_sampler   (VkhImage img, VkFilter magFilter, VkFilter minFilter,
									VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode);
vkh_public
void vkh_image_set_layout       (VkCommandBuffer cmdBuff, VkhImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
									VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);
vkh_public
void vkh_image_set_layout_subres(VkCommandBuffer cmdBuff, VkhImage image, VkImageSubresourceRange subresourceRange, VkImageLayout old_image_layout,
									VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);
vkh_public
void vkh_image_destroy_sampler  (VkhImage img);
vkh_public
void vkh_image_destroy          (VkhImage img);
vkh_public
void vkh_image_reference		(VkhImage img);
vkh_public
void* vkh_image_map             (VkhImage img);
vkh_public
void vkh_image_unmap            (VkhImage img);
vkh_public
void vkh_image_set_name         (VkhImage img, const char* name);
vkh_public
uint64_t vkh_image_get_stride	(VkhImage img);

vkh_public
VkImage                 vkh_image_get_vkimage   (VkhImage img);
vkh_public
VkImageView             vkh_image_get_view      (VkhImage img);
vkh_public
VkImageLayout           vkh_image_get_layout    (VkhImage img);
vkh_public
VkSampler               vkh_image_get_sampler   (VkhImage img);
vkh_public
VkDescriptorImageInfo   vkh_image_get_descriptor(VkhImage img, VkImageLayout imageLayout);

/*************
 * VkhBuffer *
 *************/
vkh_public
VkhBuffer   vkh_buffer_create   (VkhDevice pDev, VkBufferUsageFlags usage,
									VmaMemoryUsage memprops, VkDeviceSize size);
vkh_public
void        vkh_buffer_destroy  (VkhBuffer buff);
vkh_public
VkResult    vkh_buffer_map      (VkhBuffer buff);
vkh_public
void        vkh_buffer_unmap    (VkhBuffer buff);

vkh_public
VkBuffer    vkh_buffer_get_vkbuffer         (VkhBuffer buff);
vkh_public
void*       vkh_buffer_get_mapped_pointer   (VkhBuffer buff);

vkh_public
VkFence         vkh_fence_create			(VkhDevice dev);
vkh_public
VkFence         vkh_fence_create_signaled	(VkhDevice dev);
vkh_public
VkSemaphore     vkh_semaphore_create		(VkhDevice dev);
vkh_public
VkEvent         vkh_event_create            (VkhDevice dev);

vkh_public
VkCommandPool   vkh_cmd_pool_create (VkhDevice dev, uint32_t qFamIndex, VkCommandPoolCreateFlags flags);
vkh_public
VkCommandBuffer vkh_cmd_buff_create (VkhDevice dev, VkCommandPool cmdPool, VkCommandBufferLevel level);
vkh_public
void vkh_cmd_buffs_create (VkhDevice dev, VkCommandPool cmdPool, VkCommandBufferLevel level, uint32_t count, VkCommandBuffer* cmdBuffs);
vkh_public
void vkh_cmd_begin  (VkCommandBuffer cmdBuff, VkCommandBufferUsageFlags flags);
vkh_public
void vkh_cmd_end    (VkCommandBuffer cmdBuff);
vkh_public
void vkh_cmd_submit (VkhQueue queue, VkCommandBuffer *pCmdBuff, VkFence fence);
vkh_public
void vkh_cmd_submit_with_semaphores(VkhQueue queue, VkCommandBuffer *pCmdBuff, VkSemaphore waitSemaphore,
									VkSemaphore signalSemaphore, VkFence fence);

vkh_public
void vkh_cmd_label_start   (VkCommandBuffer cmd, const char* name, const float color[]);
vkh_public
void vkh_cmd_label_end     (VkCommandBuffer cmd);
vkh_public
void vkh_cmd_label_insert  (VkCommandBuffer cmd, const char* name, const float color[4]);

vkh_public
VkShaderModule vkh_load_module(VkDevice dev, const char* path);

vkh_public
bool        vkh_memory_type_from_properties(VkPhysicalDeviceMemoryProperties* memory_properties, uint32_t typeBits,
										VkFlags requirements_mask, uint32_t *typeIndex);
vkh_public
char *      read_spv(const char *filename, size_t *psize);
vkh_public
uint32_t*   readFile(uint32_t* length, const char* filename);

vkh_public
void dumpLayerExts ();

vkh_public
void        set_image_layout(VkCommandBuffer cmdBuff, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
					  VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);
vkh_public
void        set_image_layout_subres(VkCommandBuffer cmdBuff, VkImage image, VkImageSubresourceRange subresourceRange, VkImageLayout old_image_layout,
					  VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);
/////////////////////
vkh_public
VkhQueue    vkh_queue_create    (VkhDevice dev, uint32_t familyIndex, uint32_t qIndex);
vkh_public
void        vkh_queue_destroy   (VkhQueue queue);
//VkhQueue    vkh_queue_find      (VkhDevice dev, VkQueueFlags flags);
/////////////////////

vkh_public
bool vkh_instance_extension_supported (const char* instanceName);
vkh_public
void vkh_instance_extensions_check_init ();
vkh_public
void vkh_instance_extensions_check_release ();
vkh_public
bool vkh_layer_is_present (const char* layerName);
vkh_public
void vkh_layers_check_init ();
vkh_public
void vkh_layers_check_release ();
#ifdef __cplusplus
}
#endif

#endif
