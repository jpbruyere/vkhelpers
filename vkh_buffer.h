#ifndef VKH_BUFFER_H
#define VKH_BUFFER_H

#include <vulkan/vulkan.h>
#include "vkh_device.h"

typedef struct vkh_buffer_t {
	vkh_device* pDev;
	VkBuffer buffer;
	VkDeviceMemory memory;
	VkDescriptorBufferInfo descriptor;
	VkDeviceSize size;
	VkDeviceSize alignment;

	VkBufferUsageFlags usageFlags;
	VkMemoryPropertyFlags memoryPropertyFlags;

	void* mapped;
}vkh_buffer;

void vkh_buffer_create (vkh_device *pDev, VkBufferUsageFlags usage,
		VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, vkh_buffer* buff);
void vkh_buffer_destroy (vkh_buffer* buff);
VkResult vkh_buffer_map (vkh_buffer* buff);
void vkh_buffer_unmap (vkh_buffer* buff);
VkResult vkh_buffer_bind (vkh_buffer* buff);

#endif
