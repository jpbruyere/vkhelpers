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
#include "vkh_phyinfo.h"
#include "vkh_app.h"


VkhPhyInfo vkh_phyinfo_create (VkPhysicalDevice phy, VkSurfaceKHR surface) {
	VkhPhyInfo pi = (vkh_phy_t*)calloc(1, sizeof(vkh_phy_t));
	pi->phy = phy;

	vkGetPhysicalDeviceProperties (phy, &pi->properties);
	vkGetPhysicalDeviceMemoryProperties (phy, &pi->memProps);

	vkGetPhysicalDeviceQueueFamilyProperties (phy, &pi->queueCount, NULL);
	pi->queues = (VkQueueFamilyProperties*)malloc(pi->queueCount * sizeof(VkQueueFamilyProperties));
	vkGetPhysicalDeviceQueueFamilyProperties (phy, &pi->queueCount, pi->queues);

	//identify dedicated queues

	pi->cQueue = -1;
	pi->gQueue = -1;
	pi->tQueue = -1;
	pi->pQueue = -1;

	//try to find dedicated queues first
	for (uint32_t j=0; j<pi->queueCount; j++){
		VkBool32 present = VK_FALSE;
		switch (pi->queues[j].queueFlags) {
		case VK_QUEUE_GRAPHICS_BIT:
			if (surface)
				vkGetPhysicalDeviceSurfaceSupportKHR(phy, j, surface, &present);
			if (present){
				if (pi->pQueue<0)
					pi->pQueue = j;
			}else if (pi->gQueue<0)
				pi->gQueue = j;
			break;
		case VK_QUEUE_COMPUTE_BIT:
			if (pi->cQueue<0)
				pi->cQueue = j;
			break;
		case VK_QUEUE_TRANSFER_BIT:
			if (pi->tQueue<0)
				pi->tQueue = j;
			break;
		}
	}
	//try to find suitable queue if no dedicated one found
	for (uint32_t j=0; j<pi->queueCount; j++){
		if (pi->queues[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			VkBool32 present = 0;
			if (surface)
				vkGetPhysicalDeviceSurfaceSupportKHR(phy, j, surface, &present);
			//printf ("surf=%d, q=%d, present=%d\n",surface,j,present);
			if (present){
				if (pi->pQueue<0)
					pi->pQueue = j;
			}else if (pi->gQueue<0)
				pi->gQueue = j;
		}
		if ((pi->queues[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) && (pi->gQueue < 0))
			pi->gQueue = j;
		if ((pi->queues[j].queueFlags & VK_QUEUE_COMPUTE_BIT) && (pi->cQueue < 0))
			pi->cQueue = j;
		if ((pi->queues[j].queueFlags & VK_QUEUE_TRANSFER_BIT) && (pi->tQueue < 0))
			pi->tQueue = j;
	}

	return pi;
}

void vkh_phyinfo_destroy (VkhPhyInfo phy) {
	if (phy->pExtensionProperties != NULL)
		free(phy->pExtensionProperties);
	free(phy->queues);
	free(phy);
}

VkPhysicalDeviceProperties vkh_phyinfo_get_properties (VkhPhyInfo phy) {
	return phy->properties;
}
VkPhysicalDeviceMemoryProperties vkh_phyinfo_get_memory_properties (VkhPhyInfo phy) {
	return phy->memProps;
}

void vkh_phyinfo_get_queue_fam_indices (VkhPhyInfo phy, int* pQueue, int* gQueue, int* tQueue, int* cQueue) {
	if (pQueue)	*pQueue = phy->pQueue;
	if (gQueue)	*gQueue = phy->gQueue;
	if (tQueue)	*tQueue = phy->tQueue;
	if (cQueue)	*cQueue = phy->cQueue;
}
VkQueueFamilyProperties* vkh_phyinfo_get_queues_props(VkhPhyInfo phy, uint32_t* qCount) {
	*qCount = phy->queueCount;
	return phy->queues;
}
bool vkh_phyinfo_create_queues (VkhPhyInfo phy, int qFam, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo) {
	qInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	if (phy->queues[qFam].queueCount < queueCount)
		fprintf(stderr, "Request %d queues of family %d, but only %d available\n", queueCount, qFam, phy->queues[qFam].queueCount);
	else {
		qInfo->queueCount = queueCount,
		qInfo->queueFamilyIndex = qFam,
		qInfo->pQueuePriorities = queue_priorities;
		phy->queues[qFam].queueCount -= queueCount;
		return true;
	}
	return false;
}
bool vkh_phyinfo_create_presentable_queues (VkhPhyInfo phy, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo) {
	qInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	if (phy->pQueue < 0)
		perror("No queue with presentable support found");
	else if (phy->queues[phy->pQueue].queueCount < queueCount)
		fprintf(stderr, "Request %d queues of family %d, but only %d available\n", queueCount, phy->pQueue, phy->queues[phy->pQueue].queueCount);
	else {
		qInfo->queueCount = queueCount,
		qInfo->queueFamilyIndex = phy->pQueue,
		qInfo->pQueuePriorities = queue_priorities;
		phy->queues[phy->pQueue].queueCount -= queueCount;
		return true;
	}
	return false;
}
bool vkh_phyinfo_create_transfer_queues (VkhPhyInfo phy, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo) {
	qInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	if (phy->tQueue < 0)
		perror("No transfer queue found");
	else if (phy->queues[phy->tQueue].queueCount < queueCount)
		fprintf(stderr, "Request %d transfer queues of family %d, but only %d available\n", queueCount, phy->tQueue, phy->queues[phy->tQueue].queueCount);
	else {
		qInfo->queueCount = queueCount;
		qInfo->queueFamilyIndex = phy->tQueue;
		qInfo->pQueuePriorities = queue_priorities;
		phy->queues[phy->tQueue].queueCount -= queueCount;
		return true;
	}
	return false;
}
bool vkh_phyinfo_create_compute_queues(VkhPhyInfo phy, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo) {
	qInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	if (phy->cQueue < 0)
		perror("No compute queue found");
	else if (phy->queues[phy->cQueue].queueCount < queueCount)
		fprintf(stderr, "Request %d compute queues of family %d, but only %d available\n", queueCount, phy->cQueue, phy->queues[phy->cQueue].queueCount);
	else {
		qInfo->queueCount = queueCount,
		qInfo->queueFamilyIndex = phy->cQueue,
		qInfo->pQueuePriorities = queue_priorities;
		phy->queues[phy->cQueue].queueCount -= queueCount;
		return true;
	}
	return false;
}
bool vkh_phy_info_create_graphic_queues (VkhPhyInfo phy, uint32_t queueCount, const float* queue_priorities, VkDeviceQueueCreateInfo* const qInfo) {
	qInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	if (phy->gQueue < 0)
		perror("No graphic queue found");
	else if (phy->queues[phy->gQueue].queueCount < queueCount)
		fprintf(stderr, "Request %d graphic queues of family %d, but only %d available\n", queueCount, phy->gQueue, phy->queues[phy->gQueue].queueCount);
	else {
		qInfo->queueCount = queueCount,
		qInfo->queueFamilyIndex = phy->gQueue,
		qInfo->pQueuePriorities = queue_priorities;
		phy->queues[phy->gQueue].queueCount -= queueCount;
		return true;
	}
	return false;
}
bool vkh_phyinfo_try_get_extension_properties (VkhPhyInfo phy, const char* name, const VkExtensionProperties* properties) {
	if (phy->pExtensionProperties == NULL) {
		VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(phy->phy, NULL, &phy->extensionCount, NULL));
		phy->pExtensionProperties = (VkExtensionProperties*)malloc(phy->extensionCount * sizeof(VkExtensionProperties));
		VK_CHECK_RESULT(vkEnumerateDeviceExtensionProperties(phy->phy, NULL, &phy->extensionCount, phy->pExtensionProperties));
	}
	for (uint32_t i=0; i<phy->extensionCount; i++) {
		if (strcmp(name, phy->pExtensionProperties[i].extensionName)==0) {
			if (properties)
				properties = &phy->pExtensionProperties[i];
			return true;
		}
	}
	properties = NULL;
	return false;
}
