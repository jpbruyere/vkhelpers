/*
 * Copyright (c) 2018-2022 Jean-Philippe Bruyère <jp_bruyere@hotmail.com>
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

#ifndef VKH_USE_VMA
void _set_size_and_bind(VkhDevice pDev, VkBufferUsageFlags usage, VkhMemoryUsage memoryUsage, VkDeviceSize size, VkhBuffer buff){
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(pDev->dev, buff->buffer, &memReq);
	VkMemoryAllocateInfo memAllocInfo = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
										  .allocationSize = memReq.size };
	assert(vkh_memory_type_from_properties(&pDev->phyMemProps, memReq.memoryTypeBits, memoryUsage, &memAllocInfo.memoryTypeIndex) == true);
	VK_CHECK_RESULT(vkAllocateMemory(pDev->dev, &memAllocInfo, NULL, &buff->memory));

	buff->alignment = memReq.alignment;
	buff->size = memAllocInfo.allocationSize;
	buff->usageFlags = usage;
	buff->memprops = memoryUsage;

	VK_CHECK_RESULT(vkBindBufferMemory(buff->pDev->dev, buff->buffer, buff->memory, 0));
}
#endif

void vkh_buffer_init(VkhDevice pDev, VkBufferUsageFlags usage, VkhMemoryUsage memprops, VkDeviceSize size, VkhBuffer buff, bool mapped){
	buff->pDev			= pDev;
	VkBufferCreateInfo* pInfo = &buff->infos;
	pInfo->sType		= VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	pInfo->usage		= usage;
	pInfo->size			= size;
	pInfo->sharingMode	= VK_SHARING_MODE_EXCLUSIVE;
#ifdef VKH_USE_VMA	
	buff->allocCreateInfo.usage	= (VmaMemoryUsage)memprops;
	if (mapped)
		buff->allocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	VK_CHECK_RESULT(vmaCreateBuffer(pDev->allocator, pInfo, &buff->allocCreateInfo, &buff->buffer, &buff->alloc, &buff->allocInfo));
#else
	VK_CHECK_RESULT(vkCreateBuffer(pDev->dev, pInfo, NULL, &buff->buffer));	
	_set_size_and_bind (pDev, usage, memprops, size, buff);
	buff->memprops = memprops;
	if (mapped)
		VK_CHECK_RESULT(vkMapMemory(buff->pDev->dev, buff->memory, 0, VK_WHOLE_SIZE, 0, &buff->mapped));
#endif
}

VkhBuffer vkh_buffer_create(VkhDevice pDev, VkBufferUsageFlags usage, VkhMemoryUsage memprops, VkDeviceSize size){
	VkhBuffer buff = (VkhBuffer)calloc(1, sizeof(vkh_buffer_t));
	vkh_buffer_init(pDev, usage, memprops, size, buff, false);
	return buff;
}

void vkh_buffer_reset(VkhBuffer buff){
	if (buff->buffer)
#ifdef VKH_USE_VMA
		vmaDestroyBuffer(buff->pDev->allocator, buff->buffer, buff->alloc);
#else
		vkDestroyBuffer(buff->pDev->dev, buff->buffer, NULL);
	if (buff->memory)
		vkFreeMemory(buff->pDev->dev, buff->memory, NULL);
#endif
}
void vkh_buffer_destroy(VkhBuffer buff){
	if (buff->buffer)
#ifdef VKH_USE_VMA
		vmaDestroyBuffer(buff->pDev->allocator, buff->buffer, buff->alloc);
#else
		vkDestroyBuffer(buff->pDev->dev, buff->buffer, NULL);
	if (buff->memory)
		vkFreeMemory(buff->pDev->dev, buff->memory, NULL);
#endif
	free(buff);
	buff = NULL;
}
void vkh_buffer_resize(VkhBuffer buff, VkDeviceSize newSize, bool mapped){
	vkh_buffer_reset(buff);
	buff->infos.size = newSize;
#ifdef VKH_USE_VMA
	VK_CHECK_RESULT(vmaCreateBuffer(buff->pDev->allocator, &buff->infos, &buff->allocCreateInfo, &buff->buffer, &buff->alloc, &buff->allocInfo));
#else
	VK_CHECK_RESULT(vkCreateBuffer(buff->pDev->dev, &buff->infos, NULL, &buff->buffer));
	_set_size_and_bind (buff->pDev, buff->usageFlags, buff->memprops, buff->infos.size, buff);
	if (mapped)
		VK_CHECK_RESULT(vkMapMemory(buff->pDev->dev, buff->memory, 0, VK_WHOLE_SIZE, 0, &buff->mapped));
#endif
}

VkDescriptorBufferInfo vkh_buffer_get_descriptor (VkhBuffer buff){
	VkDescriptorBufferInfo desc = {
		.buffer = buff->buffer,
		.offset = 0,
		.range	= VK_WHOLE_SIZE};
	return desc;
}


VkResult vkh_buffer_map(VkhBuffer buff){
#ifdef VKH_USE_VMA
	return vmaMapMemory(buff->pDev->allocator, buff->alloc, &buff->mapped);
#else
	return vkMapMemory(buff->pDev->dev, buff->memory, 0, VK_WHOLE_SIZE, 0, &buff->mapped);
#endif
}
void vkh_buffer_unmap(VkhBuffer buff){
#ifdef VKH_USE_VMA
	vmaUnmapMemory(buff->pDev->allocator, buff->alloc);
#else
	if (!buff->mapped)
		return;
	vkUnmapMemory (buff->pDev->dev, buff->memory);
	buff->mapped = NULL;
#endif
}
VkBuffer vkh_buffer_get_vkbuffer (VkhBuffer buff){
	return buff->buffer;
}
void* vkh_buffer_get_mapped_pointer (VkhBuffer buff){
#ifdef VKH_USE_VMA
	//vmaFlushAllocation (buff->pDev->allocator, buff->alloc, buff->allocInfo.offset, buff->allocInfo.size);
	return buff->allocInfo.pMappedData;
#else
	return buff->mapped;
#endif
}
void vkh_buffer_flush (VkhBuffer buff){
#ifdef VKH_USE_VMA
	vmaFlushAllocation (buff->pDev->allocator, buff->alloc, buff->allocInfo.offset, buff->allocInfo.size);
#else
#endif
}
