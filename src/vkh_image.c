/*
 * Copyright (c) 2018 Jean-Philippe Bruyère <jp_bruyere@hotmail.com>
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
#include "vkh_image.h"
#include "vkh_device.h"

VkhImage _vkh_image_create (VkhDevice pDev, VkImageType imageType,
                  VkFormat format, uint32_t width, uint32_t height,
                  VmaMemoryUsage memprops, VkImageUsageFlags usage,
                  VkSampleCountFlagBits samples, VkImageTiling tiling,
                  uint32_t mipLevels, uint32_t arrayLayers){
    VkhImage img = (VkhImage)calloc(1,sizeof(vkh_image_t));

    img->pDev = pDev;
    img->width = width;
    img->height = height;
    img->layout = VK_IMAGE_LAYOUT_UNDEFINED;
    img->format = format;
    img->layers = arrayLayers;
    img->mipLevels = mipLevels;
    img->imported = false;
    img->alloc = VK_NULL_HANDLE;
    img->image = VK_NULL_HANDLE;

    VkImageCreateInfo image_info = { .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                                     .imageType = imageType,
                                     .tiling = tiling,
                                     .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
                                     .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                                     .usage = usage,
                                     .format = format,
                                     .extent = {width,height,1},
                                     .mipLevels = mipLevels,
                                     .arrayLayers = arrayLayers,
                                     .samples = samples };

    VmaAllocationCreateInfo allocInfo = { .usage = memprops };
    VK_CHECK_RESULT(vmaCreateImage (pDev->allocator, &image_info, &allocInfo, &img->image, &img->alloc, &img->allocInfo));

    /*VK_CHECK_RESULT(vkCreateImage(pDev->dev, &image_info, NULL, &img->image));

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(pDev->dev, img->image, &memReq);

    VkMemoryAllocateInfo memAllocInfo = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                          .allocationSize = memReq.size };
    assert(vkh_memory_type_from_properties(&pDev->phyMemProps, memReq.memoryTypeBits, memprops,&memAllocInfo.memoryTypeIndex));

    VK_CHECK_RESULT(vkAllocateMemory(pDev->dev, &memAllocInfo, NULL, &img->memory));
    VK_CHECK_RESULT(vkBindImageMemory(pDev->dev, img->image, img->memory, offset));*/

    return img;
}
VkhImage vkh_tex2d_array_create (VkhDevice pDev,
                             VkFormat format, uint32_t width, uint32_t height, uint32_t layers,
                             VmaMemoryUsage memprops, VkImageUsageFlags usage){
    return _vkh_image_create (pDev, VK_IMAGE_TYPE_2D, format, width, height, memprops,usage,
        VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, 1, layers);
}
VkhImage vkh_image_create (VkhDevice pDev,
                           VkFormat format, uint32_t width, uint32_t height, VkImageTiling tiling,
                           VmaMemoryUsage memprops,
                           VkImageUsageFlags usage)
{
    return _vkh_image_create (pDev, VK_IMAGE_TYPE_2D, format, width, height, memprops,usage,
                      VK_SAMPLE_COUNT_1_BIT, tiling, 1, 1);
}
//create vkhImage from existing VkImage
VkhImage vkh_image_import (VkhDevice pDev, VkImage vkImg, VkFormat format, uint32_t width, uint32_t height) {
    VkhImage img = (VkhImage)calloc(1,sizeof(vkh_image_t));
    img->pDev = pDev;
    img->image = vkImg;
    img->format = format;
    img->width = width;
    img->height = height;
    img->imported = true;
    return img;
}
VkhImage vkh_image_ms_create(VkhDevice pDev,
                           VkFormat format, VkSampleCountFlagBits num_samples, uint32_t width, uint32_t height,
                           VmaMemoryUsage memprops,
                           VkImageUsageFlags usage){
   return  _vkh_image_create (pDev, VK_IMAGE_TYPE_2D, format, width, height, memprops,usage,
                      num_samples, VK_IMAGE_TILING_OPTIMAL, 1, 1);
}
void vkh_image_create_view (VkhImage img, VkImageViewType viewType, VkImageAspectFlags aspectFlags){
    if(img->view != VK_NULL_HANDLE)
        vkDestroyImageView  (img->pDev->dev,img->view,NULL);

    VkImageViewCreateInfo viewInfo = { .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                                         .image = img->image,
                                         .viewType = viewType,
                                         .format = img->format,
                                         .components = {VK_COMPONENT_SWIZZLE_R,VK_COMPONENT_SWIZZLE_G,VK_COMPONENT_SWIZZLE_B,VK_COMPONENT_SWIZZLE_A},
                                         .subresourceRange = {aspectFlags,0,1,0,img->layers}};
    VK_CHECK_RESULT(vkCreateImageView(img->pDev->dev, &viewInfo, NULL, &img->view));
}
void vkh_image_create_sampler (VkhImage img, VkFilter magFilter, VkFilter minFilter,
                               VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode){
    if(img->sampler != VK_NULL_HANDLE)
        vkDestroySampler    (img->pDev->dev,img->sampler,NULL);
    VkSamplerCreateInfo samplerCreateInfo = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
                                              .maxAnisotropy= 1.0,
                                              .addressModeU = addressMode,
                                              .addressModeV = addressMode,
                                              .addressModeW = addressMode,
                                              .magFilter    = magFilter,
                                              .minFilter    = minFilter,
                                              .mipmapMode   = mipmapMode};
    VK_CHECK_RESULT(vkCreateSampler(img->pDev->dev, &samplerCreateInfo, NULL, &img->sampler));
}

void vkh_image_create_descriptor(VkhImage img, VkImageViewType viewType, VkImageAspectFlags aspectFlags, VkFilter magFilter,
                                 VkFilter minFilter, VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode)
{
    vkh_image_create_view       (img, viewType, aspectFlags);
    vkh_image_create_sampler    (img, magFilter, minFilter, mipmapMode, addressMode);
}
VkImage vkh_image_get_vkimage (VkhImage img){
    return img->image;
}
VkSampler vkh_image_get_sampler (VkhImage img){
    return img->sampler;
}
VkImageView vkh_image_get_view (VkhImage img){
    return img->view;
}
VkImageLayout vkh_image_get_layout (VkhImage img){
    return img->layout;
}
VkDescriptorImageInfo vkh_image_get_descriptor (VkhImage img, VkImageLayout imageLayout){
    VkDescriptorImageInfo desc = { .imageView = img->view,
                                   .imageLayout = imageLayout,
                                   .sampler = img->sampler };
    return desc;
}

void vkh_image_set_layout(VkCommandBuffer cmdBuff, VkhImage image, VkImageAspectFlags aspectMask, VkImageLayout new_image_layout,
                      VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages) {
    VkImageSubresourceRange subres = {aspectMask,0,1,0,1};
    vkh_image_set_layout_subres(cmdBuff, image, subres, new_image_layout, src_stages, dest_stages);
}

void vkh_image_set_layout_subres(VkCommandBuffer cmdBuff, VkhImage image, VkImageSubresourceRange subresourceRange,
                             VkImageLayout new_image_layout,
                             VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages) {
    VkImageMemoryBarrier image_memory_barrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                  .oldLayout = image->layout,
                                                  .newLayout = new_image_layout,
                                                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                  .image = image->image,
                                                  .subresourceRange = subresourceRange};

    switch (image->layout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;
        default:
            break;
    }

    switch (new_image_layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;
        default:
            break;
    }

    vkCmdPipelineBarrier(cmdBuff, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
    image->layout = new_image_layout;
}

void vkh_image_destroy(VkhImage img)
{
    if(img->view != VK_NULL_HANDLE)
        vkDestroyImageView  (img->pDev->dev,img->view,NULL);
    if(img->sampler != VK_NULL_HANDLE)
        vkDestroySampler    (img->pDev->dev,img->sampler,NULL);

    if (!img->imported)
        vmaDestroyImage     (img->pDev->allocator, img->image, img->alloc);

    free(img);
    img = NULL;
}

void* vkh_image_map (VkhImage img) {
    void* data;
    vmaMapMemory(img->pDev->allocator, img->alloc, &data);
    return data;
}
void vkh_image_unmap (VkhImage img) {
    vmaUnmapMemory(img->pDev->allocator, img->alloc);
}
