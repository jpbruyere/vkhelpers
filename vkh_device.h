#ifndef VKH_DEVICE_H
#define VKH_DEVICE_H

#include "vkhelpers.h"

typedef struct vkh_device_t{
	VkDevice vkDev;
	VkPhysicalDeviceMemoryProperties phyMemProps;
	VkRenderPass renderPass;
}vkh_device;

#endif
