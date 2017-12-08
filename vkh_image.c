#include "vkh_image.h"

void _vkh_image_create (vkh_device *pDev, VkImageType imageType,
                  VkFormat format, uint32_t width, uint32_t height,
                  VkMemoryPropertyFlags memprops, VkImageUsageFlags usage,
                  VkSampleCountFlagBits samples, VkImageTiling tiling,
                  uint32_t mipLevels, uint32_t arrayLayers,
                  VkImageLayout layout, vkh_image* img){
    img->pDev = pDev;
    img->width = width;
    img->height = height;

    VkImageCreateInfo image_info = { .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                     .imageType = imageType,
                                     .tiling = tiling,
                                     .initialLayout = layout,
                                     .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                     .usage = usage,
                                     .format = format,
                                     .extent = {width,height,1},
                                     .mipLevels = mipLevels,
                                     .arrayLayers = arrayLayers,
                                     .samples = samples };

    VK_CHECK_RESULT(vkCreateImage(pDev->vkDev, &image_info, NULL, &img->image));

    img->infos = image_info;

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(pDev->vkDev, img->image, &memReq);
    VkMemoryAllocateInfo memAllocInfo = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                          .allocationSize = memReq.size };
    assert(memory_type_from_properties(&pDev->phyMemProps, memReq.memoryTypeBits, memprops,&memAllocInfo.memoryTypeIndex));
    VK_CHECK_RESULT(vkAllocateMemory(pDev->vkDev, &memAllocInfo, NULL, &img->memory));
    VK_CHECK_RESULT(vkBindImageMemory(pDev->vkDev, img->image, img->memory, 0));
}
void vkh_tex2d_array_create (vkh_device *pDev,
                             VkFormat format, uint32_t width, uint32_t height, uint32_t layers,
                             VkMemoryPropertyFlags memprops, VkImageUsageFlags usage, vkh_image* img){
    _vkh_image_create (pDev, VK_IMAGE_TYPE_2D, format, width, height, memprops,usage,
        VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, 1, layers, VK_IMAGE_LAYOUT_PREINITIALIZED, img);
}

void vkh_image_create (vkh_device *pDev,
                           VkFormat format, uint32_t width, uint32_t height, VkImageTiling tiling,
                           VkMemoryPropertyFlags memprops,
                           VkImageUsageFlags usage, VkImageLayout layout, vkh_image* img)
{
    _vkh_image_create (pDev, VK_IMAGE_TYPE_2D, format, width, height, memprops,usage,
                      VK_SAMPLE_COUNT_1_BIT, tiling, 1, 1, layout, img);
}
void vkh_image_ms_create (vkh_device *pDev,
                           VkFormat format, VkSampleCountFlagBits num_samples, uint32_t width, uint32_t height,
                           VkMemoryPropertyFlags memprops,
                           VkImageUsageFlags usage, VkImageLayout layout, vkh_image* img){
    _vkh_image_create (pDev, VK_IMAGE_TYPE_2D, format, width, height, memprops,usage,
                      num_samples, VK_IMAGE_TILING_OPTIMAL, 1, 1, layout, img);
}
void vkh_image_create_descriptor(vkh_image* img, VkImageViewType viewType, VkImageAspectFlags aspectFlags, VkFilter magFilter,
                                 VkFilter minFilter, VkSamplerMipmapMode mipmapMode)
{
    img->pDescriptor = (VkDescriptorImageInfo*)malloc(sizeof(VkDescriptorImageInfo));
    img->pDescriptor->imageLayout = img->infos.initialLayout;
    VkImageViewCreateInfo viewInfo = { .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                         .image = img->image,
                                         .viewType = viewType,
                                         .format = img->infos.format,
                                         .components = {VK_COMPONENT_SWIZZLE_R,VK_COMPONENT_SWIZZLE_G,VK_COMPONENT_SWIZZLE_B,VK_COMPONENT_SWIZZLE_A},
                                         .subresourceRange = {aspectFlags,0,1,0,img->infos.arrayLayers}};
    VK_CHECK_RESULT(vkCreateImageView(img->pDev->vkDev, &viewInfo, NULL, &img->pDescriptor->imageView));

    VkSamplerCreateInfo samplerCreateInfo = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                              .maxAnisotropy = 1.0,
                                              .magFilter = magFilter,
                                              .minFilter = minFilter,
                                              .mipmapMode = mipmapMode};
    VK_CHECK_RESULT(vkCreateSampler(img->pDev->vkDev, &samplerCreateInfo, NULL, &img->pDescriptor->sampler));

}
void vkh_image_destroy(vkh_image* img)
{
    if (img->pDescriptor != NULL){
        vkDestroyImageView(img->pDev->vkDev,img->pDescriptor->imageView,NULL);
        if(img->pDescriptor->sampler != VK_NULL_HANDLE)
            vkDestroySampler(img->pDev->vkDev,img->pDescriptor->sampler,NULL);
    }
    free(img->pDescriptor);
    vkDestroyImage(img->pDev->vkDev,img->image,NULL);
    vkFreeMemory(img->pDev->vkDev, img->memory, NULL);
}
