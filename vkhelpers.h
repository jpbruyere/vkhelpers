#ifndef VK_HELPERS_H
#define VK_HELPERS_H

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

VkPhysicalDevice vkh_find_phy (VkInstance inst, VkPhysicalDeviceType phyType);
VkFence vkh_fence_create (VkDevice dev);
VkSemaphore vkh_semaphore_create (VkDevice dev);
VkCommandPool vkh_cmd_pool_create (VkDevice dev, uint32_t qFamIndex, VkCommandPoolCreateFlags flags);
VkCommandBuffer vkh_cmd_buff_create (VkDevice dev, VkCommandPool cmdPool, VkCommandBufferLevel level);
void vkh_cmd_begin(VkCommandBuffer cmdBuff, VkCommandBufferUsageFlags flags);
void vkh_cmd_end(VkCommandBuffer cmdBuff);
void vkh_cmd_submit(VkQueue queue, VkCommandBuffer *pCmdBuff, VkFence fence);
void vkh_cmd_submit_with_semaphores(VkQueue queue, VkCommandBuffer *pCmdBuff, VkSemaphore waitSemaphore,
                                    VkSemaphore signalSemaphore, VkFence fence);
void vkcrow_cmd_copy_submit(VkQueue queue, VkCommandBuffer *pCmdBuff, VkSemaphore* pWaitSemaphore, VkSemaphore* pSignalSemaphore);

VkShaderModule vkh_load_module(VkDevice dev, const char* path);

void set_image_layout(VkCommandBuffer cmdBuff, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
                      VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages);

bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties* memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex);
char *read_spv(const char *filename, size_t *psize);
uint32_t* readFile(uint32_t* length, const char* filename);

void dumpLayerExts ();
#endif
