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
#ifndef VKH_DEVICE_H
#define VKH_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "vkh.h"

#ifdef VKH_USE_VMA
#include "vk_mem_alloc.h"
#endif

typedef struct _vkh_device_t{
	VkDevice				dev;
	VkPhysicalDeviceMemoryProperties phyMemProps;
	VkPhysicalDevice		phy;
	VkInstance				instance;
#ifdef VKH_USE_VMA
	VmaAllocator			allocator;
#endif
	VkhApp					vkhApplication;
}vkh_device_t;

#ifdef __cplusplus
}
#endif
#endif
