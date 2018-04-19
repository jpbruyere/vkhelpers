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
#include "vkh_buffer.h"
#include "vkh_device.h"

VkhBuffer vkh_buffer_create(VkhDevice pDev, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size){
    VkhBuffer buff = (VkhBuffer)malloc(sizeof(vkh_buffer_t));
    buff->pDev = pDev;
    VkBufferCreateInfo bufCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .usage = usage, .size = size, .sharingMode = VK_SHARING_MODE_EXCLUSIVE};
    VK_CHECK_RESULT(vkCreateBuffer(pDev->dev, &bufCreateInfo, NULL, &buff->buffer));

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(pDev->dev, buff->buffer, &memReq);
    VkMemoryAllocateInfo memAllocInfo = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                          .allocationSize = memReq.size };
    assert(memory_type_from_properties(&pDev->phyMemProps, memReq.memoryTypeBits,memoryPropertyFlags, &memAllocInfo.memoryTypeIndex));
    VK_CHECK_RESULT(vkAllocateMemory(pDev->dev, &memAllocInfo, NULL, &buff->memory));

    buff->alignment = memReq.alignment;
    buff->size = memAllocInfo.allocationSize;
    buff->usageFlags = usage;
    buff->memoryPropertyFlags = memoryPropertyFlags;

    VK_CHECK_RESULT(vkh_buffer_bind(buff));
    return buff;
}

void vkh_buffer_destroy(VkhBuffer buff){
    if (buff->buffer)
        vkDestroyBuffer(buff->pDev->dev, buff->buffer, NULL);
    if (buff->memory)
        vkFreeMemory(buff->pDev->dev, buff->memory, NULL);
    free(buff);
    buff = NULL;
}


VkResult vkh_buffer_map(VkhBuffer buff){
    return vkMapMemory(buff->pDev->dev, buff->memory, 0, VK_WHOLE_SIZE, 0, &buff->mapped);
}
void vkh_buffer_unmap(VkhBuffer buff){
    if (!buff->mapped)
        return;
    vkUnmapMemory(buff->pDev->dev, buff->memory);
    buff->mapped = NULL;
}

VkResult vkh_buffer_bind(VkhBuffer buff)
{
    return vkBindBufferMemory(buff->pDev->dev, buff->buffer, buff->memory, 0);
}

VkBuffer vkh_buffer_get_vkbuffer (VkhBuffer buff){
    return buff->buffer;
}
void* vkh_buffer_get_mapped_pointer (VkhBuffer buff){
    return buff->mapped;
}
