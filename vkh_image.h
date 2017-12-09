#ifndef VKH_IMAGE_H
#define VKH_IMAGE_H

#include "vkhelpers.h"

typedef struct _vkh_image_t {
    VkhDevice				pDev;
    VkImageCreateInfo		infos;
    uint32_t				width, height;
    VkImage					image;
    VkDeviceMemory			memory;
    VkDescriptorImageInfo*	pDescriptor;
    VkImageLayout           layout;//used for descriptor creation
}vkh_image_t;
#endif
