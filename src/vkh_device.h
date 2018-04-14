#ifndef VKH_DEVICE_H
#define VKH_DEVICE_H

#include "vkh.h"

typedef struct _vkh_device_t{
    VkDevice				dev;
    VkPhysicalDeviceMemoryProperties phyMemProps;
    VkRenderPass			renderPass;
//    VkPhysicalDevice    phy;
//    VkDevice            dev;

//    VkPhysicalDeviceMemoryProperties phyMemProps;
}vkh_device_t;

#endif
