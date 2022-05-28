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
#include "vkh_device.h"
#include "vkh_phyinfo.h"
#include "vkh_app.h"
#include "string.h"


static PFN_vkSetDebugUtilsObjectNameEXT		SetDebugUtilsObjectNameEXT;
static PFN_vkQueueBeginDebugUtilsLabelEXT	QueueBeginDebugUtilsLabelEXT;
static PFN_vkQueueEndDebugUtilsLabelEXT		QueueEndDebugUtilsLabelEXT;
static PFN_vkCmdBeginDebugUtilsLabelEXT		CmdBeginDebugUtilsLabelEXT;
static PFN_vkCmdEndDebugUtilsLabelEXT		CmdEndDebugUtilsLabelEXT;
static PFN_vkCmdInsertDebugUtilsLabelEXT	CmdInsertDebugUtilsLabelEXT;

VkhDevice vkh_device_create (VkhApp app, VkhPhyInfo phyInfo, VkDeviceCreateInfo* pDevice_info){
	VkDevice dev;
	VK_CHECK_RESULT(vkCreateDevice (phyInfo->phy, pDevice_info, NULL, &dev));
	VkhDevice vkhd = vkh_device_import(app->inst, phyInfo->phy, dev);
	vkhd->vkhApplication = app;
	return vkhd;
}
VkhDevice vkh_device_import (VkInstance inst, VkPhysicalDevice phy, VkDevice vkDev) {
	VkhDevice dev = (vkh_device_t*)calloc(1,sizeof(vkh_device_t));
	dev->dev = vkDev;
	dev->phy = phy;
	dev->instance = inst;

	vkGetPhysicalDeviceMemoryProperties (phy, &dev->phyMemProps);
#ifdef VKH_USE_VMA
	VmaAllocatorCreateInfo allocatorInfo = {
		.physicalDevice = phy,
		.device = vkDev
	};
	vmaCreateAllocator(&allocatorInfo, &dev->allocator);
#else
#endif

	return dev;
}
VkDevice vkh_device_get_vkdev (VkhDevice dev) {
	return dev->dev;
}
VkPhysicalDevice vkh_device_get_phy (VkhDevice dev) {
	return dev->phy;
}
VkhApp vkh_device_get_app (VkhDevice dev) {
	return dev->vkhApplication;
}
/**
 * @brief get instance proc addresses for debug utils (name, label,...)
 * @param vkh device
 */
void vkh_device_init_debug_utils (VkhDevice dev) {
	SetDebugUtilsObjectNameEXT		= (PFN_vkSetDebugUtilsObjectNameEXT)	vkGetInstanceProcAddr(dev->instance, "vkSetDebugUtilsObjectNameEXT");
	QueueBeginDebugUtilsLabelEXT	= (PFN_vkQueueBeginDebugUtilsLabelEXT)	vkGetInstanceProcAddr(dev->instance, "vkQueueBeginDebugUtilsLabelEXT");
	QueueEndDebugUtilsLabelEXT		= (PFN_vkQueueEndDebugUtilsLabelEXT)	vkGetInstanceProcAddr(dev->instance, "vkQueueEndDebugUtilsLabelEXT");
	CmdBeginDebugUtilsLabelEXT		= (PFN_vkCmdBeginDebugUtilsLabelEXT)	vkGetInstanceProcAddr(dev->instance, "vkCmdBeginDebugUtilsLabelEXT");
	CmdEndDebugUtilsLabelEXT		= (PFN_vkCmdEndDebugUtilsLabelEXT)		vkGetInstanceProcAddr(dev->instance, "vkCmdEndDebugUtilsLabelEXT");
	CmdInsertDebugUtilsLabelEXT		= (PFN_vkCmdInsertDebugUtilsLabelEXT)	vkGetInstanceProcAddr(dev->instance, "vkCmdInsertDebugUtilsLabelEXT");
}
VkSampler vkh_device_create_sampler (VkhDevice dev, VkFilter magFilter, VkFilter minFilter,
							   VkSamplerMipmapMode mipmapMode, VkSamplerAddressMode addressMode){
	VkSampler sampler = VK_NULL_HANDLE;
	VkSamplerCreateInfo samplerCreateInfo = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
											  .maxAnisotropy= 1.0,
											  .addressModeU = addressMode,
											  .addressModeV = addressMode,
											  .addressModeW = addressMode,
											  .magFilter	= magFilter,
											  .minFilter	= minFilter,
											  .mipmapMode	= mipmapMode};
	VK_CHECK_RESULT(vkCreateSampler(dev->dev, &samplerCreateInfo, NULL, &sampler));
	return sampler;
}
void vkh_device_destroy_sampler (VkhDevice dev, VkSampler sampler) {
	vkDestroySampler (dev->dev, sampler, NULL);
}
void vkh_device_destroy (VkhDevice dev) {
#ifdef VKH_USE_VMA
	vmaDestroyAllocator (dev->allocator);
#else
#endif
	vkDestroyDevice (dev->dev, NULL);
	free (dev);
}

void vkh_device_set_object_name (VkhDevice dev, VkObjectType objectType, uint64_t handle, const char* name){
	const VkDebugUtilsObjectNameInfoEXT info = {
		.sType		 = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.pNext		 = 0,
		.objectType	 = objectType,
		.objectHandle= handle,
		.pObjectName = name
	};
	SetDebugUtilsObjectNameEXT (dev->dev, &info);
}
void vkh_cmd_label_start (VkCommandBuffer cmd, const char* name, const float color[4]) {
	const VkDebugUtilsLabelEXT info = {
		.sType		= VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
		.pNext		= 0,
		.pLabelName= name
	};
	memcpy ((void*)info.color, (void*)color, 4 * sizeof(float));
	CmdBeginDebugUtilsLabelEXT (cmd, &info);
}
void vkh_cmd_label_insert (VkCommandBuffer cmd, const char* name, const float color[4]) {
	const VkDebugUtilsLabelEXT info = {
		.sType		= VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
		.pNext		= 0,
		.pLabelName= name
	};
	memcpy ((void*)info.color, (void*)color, 4 * sizeof(float));
	CmdInsertDebugUtilsLabelEXT (cmd, &info);
}
void vkh_cmd_label_end (VkCommandBuffer cmd) {
	CmdEndDebugUtilsLabelEXT (cmd);
}
