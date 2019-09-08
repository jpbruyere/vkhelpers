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
    for (int j=0; j<pi->queueCount; j++){
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
    for (int j=0; j<pi->queueCount; j++){
        if (pi->queues[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            VkBool32 present;
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

    free(phy->queues);
    free(phy);
}

void vkh_phyinfo_select_queue (VkhPhyInfo phy, uint32_t qIndex, float* priorities) {

}
VkPhysicalDeviceProperties vkh_phyinfo_get_properties (VkhPhyInfo phy) {
    return phy->properties;
}
VkPhysicalDeviceMemoryProperties vkh_phyinfo_get_memory_properties (VkhPhyInfo phy) {
    return phy->memProps;
}
uint32_t vkh_phy_info_get_graphic_queue_index (VkhPhyInfo phy) {
    return (uint32_t)phy->gQueue;
}
