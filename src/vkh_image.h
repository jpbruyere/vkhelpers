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
#ifndef VKH_IMAGE_H
#define VKH_IMAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vkh.h"
#include "vk_mem_alloc.h"
#include "deps/tinycthread.h"

typedef struct _vkh_image_t {
	VkhDevice				pDev;
	VkImageCreateInfo		infos;
	VkImage					image;
#ifdef VKH_USE_VMA
	VmaAllocation			alloc;
	VmaAllocationInfo		allocInfo;
#else
	VkDeviceMemory			memory;
#endif
	VkSampler				sampler;
	VkImageView				view;
	VkImageLayout			layout; //current layout
	bool					imported;//dont destroy vkimage at end

	uint32_t				references;
	mtx_t					mutex;
}vkh_image_t;

#ifdef __cplusplus
}
#endif
#endif
