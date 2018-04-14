#ifndef VKH_IMAGE_H
#define VKH_IMAGE_H

#include "vkh.h"

typedef struct _vkh_image_t {
    VkhDevice				pDev;
    VkFormat                format;
    uint32_t                layers;
    uint32_t                mipLevels;
    uint32_t				width, height;
    VkImage					image;
    VkDeviceMemory			memory;
    VkSampler               sampler;
    VkImageView             view;
    VkImageLayout           layout; //current layout
}vkh_image_t;
#endif
