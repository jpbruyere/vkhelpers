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
#include "vkh_device.h"
#include "string.h"

#define KNRM  "\x1B[0m\x1B[40m"
#define KRED  "\x1B[31m\x1B[40m"
#define KGRN  "\x1B[32m\x1B[40m"
#define KYEL  "\x1B[33m\x1B[40m"
#define KBLU  "\x1B[34m\x1B[40m"
#define KMAG  "\x1B[35m\x1B[40m"
#define KCYN  "\x1B[36m\x1B[40m"
#define KWHT  "\x1B[37m\x1B[40m"

static VkDebugReportCallbackEXT msgCallback;
static PFN_vkDebugMarkerSetObjectNameEXT    DebugMarkerSetObjectNameEXT;
//I add debug markers here for convenience even if device is not part of the signature.
static PFN_vkCmdDebugMarkerBeginEXT         CmdDebugMarkerBegin;
static PFN_vkCmdDebugMarkerEndEXT           CmdDebugMarkerEnd;
static PFN_vkCmdDebugMarkerInsertEXT        CmdDebugMarkerInsert;

VKAPI_ATTR VkBool32 VKAPI_CALL messageCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t srcObject,
    size_t location,
    int32_t msgCode,
    const char* pLayerPrefix,
    const char* pMsg,
    void* pUserData)
{
    switch (flags) {
        case VK_DEBUG_REPORT_ERROR_BIT_EXT:
            printf ("%sERR: %s\n",KRED, pMsg);
            break;
        case VK_DEBUG_REPORT_DEBUG_BIT_EXT:
            printf ("%sDBG: %s\n",KMAG, pMsg);
            break;
        case VK_DEBUG_REPORT_WARNING_BIT_EXT:
            printf ("%sWRN: %s\n",KYEL, pMsg);
            break;
        case VK_DEBUG_REPORT_INFORMATION_BIT_EXT:
            printf ("%sNFO: %s\n",KCYN, pMsg);
            break;
        case VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT:
            printf ("%sPRF: %s\n",KWHT, pMsg);
            break;
        default:
            printf ("%sMSG: %s\n",KBLU, pMsg);
            break;
    }
    fflush(stdout);
    return VK_FALSE;
}

VkhDevice vkh_device_create (VkInstance inst, VkPhysicalDevice phy, VkDevice vkDev) {
    VkhDevice dev = (vkh_device_t*)malloc(sizeof(vkh_device_t));
    dev->dev = vkDev;
    dev->phy = phy;
    dev->instance = inst;

    vkGetPhysicalDeviceMemoryProperties (phy, &dev->phyMemProps);

    VmaAllocatorCreateInfo allocatorInfo = {
        .physicalDevice = phy,
        .device = vkDev
    };
    vmaCreateAllocator(&allocatorInfo, &dev->allocator);

    DebugMarkerSetObjectNameEXT = (PFN_vkDebugMarkerSetObjectNameEXT)vkGetInstanceProcAddr(dev->instance, "vkDebugMarkerSetObjectNameEXT");
    CmdDebugMarkerBegin         = (PFN_vkCmdDebugMarkerBeginEXT)vkGetInstanceProcAddr(dev->instance, "vkCmdDebugMarkerBeginEXT");
    CmdDebugMarkerEnd           = (PFN_vkCmdDebugMarkerEndEXT)vkGetInstanceProcAddr(dev->instance, "vkCmdDebugMarkerEndEXT");
    CmdDebugMarkerInsert        = (PFN_vkCmdDebugMarkerInsertEXT)vkGetInstanceProcAddr(dev->instance, "vkCmdDebugMarkerInsertEXT");

    return dev;
}

VkDebugReportCallbackEXT vkh_device_create_debug_report (VkhDevice dev, VkDebugReportFlagsEXT flags)  {

    CreateDebugReportCallback   = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(dev->instance, "vkCreateDebugReportCallbackEXT");
    DestroyDebugReportCallback  = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(dev->instance, "vkDestroyDebugReportCallbackEXT");

    VkDebugReportCallbackCreateInfoEXT dbgCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT,
        .flags = flags,
        .pfnCallback = (PFN_vkDebugReportCallbackEXT)messageCallback
    };

    CreateDebugReportCallback (dev->instance, &dbgCreateInfo, VK_NULL_HANDLE, &msgCallback);
    return msgCallback;
}
void vkh_device_destroy_debug_report (VkhDevice dev, VkDebugReportCallbackEXT dbgReport){
    DestroyDebugReportCallback (dev->instance, dbgReport, VK_NULL_HANDLE);
}

void vkh_device_destroy (VkhDevice dev) {
    vmaDestroyAllocator (dev->allocator);
    vkDestroyDevice (dev->dev, NULL);
    free (dev);
}

void vkh_device_set_object_name (VkhDevice dev, VkDebugReportObjectTypeEXT objectType, uint64_t handle, const char* name){
    const VkDebugMarkerObjectNameInfoEXT info = {
        .sType      = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT,
        .pNext      = 0,
        .objectType = objectType,
        .object     = handle,
        .pObjectName= name
    };
    DebugMarkerSetObjectNameEXT (dev->dev, &info);
}
void vkh_cmd_marker_start (VkCommandBuffer cmd, const char* name, float color[4]) {
    const VkDebugMarkerMarkerInfoEXT info = {
        .sType      = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT,
        .pNext      = 0,
        .pMarkerName= name
    };
    memcpy (info.color, color, 4 * sizeof(float));
    CmdDebugMarkerBegin (cmd, &info);
}
void vkh_cmd_marker_end (VkCommandBuffer cmd) {
    CmdDebugMarkerEnd (cmd);
}



