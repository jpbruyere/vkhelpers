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

VkhBuffer vkh_buffer_create(VkhDevice pDev, VkBufferUsageFlags usage, VmaMemoryUsage memprops, VkDeviceSize size){
	VkhBuffer buff = (VkhBuffer)malloc(sizeof(vkh_buffer_t));
	buff->pDev = pDev;
	VkBufferCreateInfo* pInfo = &buff->infos;
	pInfo->sType		 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	pInfo->usage		 = usage;
	pInfo->size			 = size;
	pInfo->sharingMode	 = VK_SHARING_MODE_EXCLUSIVE;

	buff->memprops = memprops;

	VmaAllocationCreateInfo allocInfo = { .usage = memprops };
	VK_CHECK_RESULT(vmaCreateBuffer(pDev->allocator, pInfo, &allocInfo, &buff->buffer, &buff->alloc, &buff->allocInfo));
	return buff;
}

void vkh_buffer_destroy(VkhBuffer buff){
	if (buff->buffer)
		vmaDestroyBuffer(buff->pDev->allocator, buff->buffer, buff->alloc);
	free(buff);
	buff = NULL;
}

VkDescriptorBufferInfo vkh_buffer_get_descriptor (VkhBuffer buff){
	VkDescriptorBufferInfo desc = {
		.buffer = buff->buffer,
		.offset = 0,
		.range	= VK_WHOLE_SIZE};
	return desc;
}


VkResult vkh_buffer_map(VkhBuffer buff){
	return vmaMapMemory(buff->pDev->allocator, buff->alloc, &buff->mapped);
}
void vkh_buffer_unmap(VkhBuffer buff){
	if (!buff->mapped)
		return;
	vmaUnmapMemory(buff->pDev->allocator, buff->alloc);
	buff->mapped = NULL;
}
VkBuffer vkh_buffer_get_vkbuffer (VkhBuffer buff){
	return buff->buffer;
}
void* vkh_buffer_get_mapped_pointer (VkhBuffer buff){
	return buff->mapped;
}
