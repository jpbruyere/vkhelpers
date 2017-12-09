#ifndef VKH_BUFFER_H
#define VKH_BUFFER_H

#include <vulkan/vulkan.h>
#include "vkhelpers.h"

typedef struct _vkh_buffer_t {
    VkhDevice               pDev;
    VkBuffer                buffer;
    VkDeviceMemory          memory;
    VkDescriptorBufferInfo  descriptor;
    VkDeviceSize            size;
    VkDeviceSize            alignment;

    VkBufferUsageFlags      usageFlags;
    VkMemoryPropertyFlags   memoryPropertyFlags;

    void*                   mapped;
}vkh_buffer_t;
#endif
