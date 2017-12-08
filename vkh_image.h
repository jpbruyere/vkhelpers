#ifndef VKH_IMAGE_H
#define VKH_IMAGE_H

#include "vkhelpers.h"
#include "vkh_device.h"

typedef struct vkh_image_t {
	vkh_device*				pDev;
	VkImageCreateInfo		infos;
	uint32_t				width, height;
	VkImage					image;
	VkDeviceMemory			memory;
	VkDescriptorImageInfo*	pDescriptor;
}vkh_image;

void vkh_image_create		(vkh_device* pDev, VkFormat format, uint32_t width, uint32_t height, VkImageTiling tiling,
								VkMemoryPropertyFlags memprops,	VkImageUsageFlags usage, VkImageLayout layout, vkh_image *img);
void vkh_image_ms_create	(vkh_device *pDev, VkFormat format, VkSampleCountFlagBits num_samples, uint32_t width, uint32_t height,
								VkMemoryPropertyFlags memprops,	VkImageUsageFlags usage, VkImageLayout layout, vkh_image *img);

void vkh_tex2d_array_create (vkh_device *pDev, VkFormat format, uint32_t width, uint32_t height, uint32_t layers,
								VkMemoryPropertyFlags memprops, VkImageUsageFlags usage, vkh_image* img);

void vkh_image_create_descriptor(vkh_image* img, VkImageViewType viewType, VkImageAspectFlags aspectFlags, VkFilter magFilter, VkFilter minFilter,
								 VkSamplerMipmapMode mipmapMode);
void vkh_image_destroy(vkh_image* img);
#endif
