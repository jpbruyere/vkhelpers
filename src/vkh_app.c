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
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "vkh_app.h"

#define ENGINE_NAME    "vkhelpers"
#define ENGINE_VERSION 1
uint32_t vkh_log_level = VKH_LOG_DEBUG;

void _vkh_log(uint32_t log_level, const char* format, ...) {
    if ((vkh_log_level) & (log_level)) {
        va_list args;
        va_start(args, format);
        vfprintf(stdout, format, args);
        va_end(args);
        fprintf(stdout, "\n");
        fflush(stdout);
    }
}

// default debug messenger callback enabled with 'vkh_app_enable_debug_messenger'
VkBool32 debugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                     VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
                                     const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {

    switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        printf(KYEL);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        printf(KRED);
        break;
    default:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        printf(KGRN);
        break;
    }
    switch (messageTypes) {
    case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
        printf("GEN: ");
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
        printf("VAL: ");
        break;
    case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
        printf("PRF: ");
        break;
    }

    printf(KNRM);
    printf("%s\n", pCallbackData->pMessage);

    fflush(stdout);
    return VK_FALSE;
}

VkhApp vkh_app_create(uint32_t version_major, uint32_t version_minor, const char *app_name, uint32_t enabledLayersCount,
                      const char **enabledLayers, uint32_t ext_count, const char *extentions[]) {
    uint32_t apiVersion;
    vkEnumerateInstanceVersion(&apiVersion);
    LOG(VKH_LOG_INFO, "VkhApp create: instance api version = %d.%d", VK_API_VERSION_MAJOR(apiVersion),
        VK_API_VERSION_MINOR(apiVersion));

    VkhApp app = (VkhApp)malloc(sizeof(vkh_app_t));
    if (!app) {
        LOG(VKH_LOG_ERR, "VkhApp creation failed, out of memory.");
        return NULL;
    }

    VkApplicationInfo infos = {.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                               .pApplicationName   = app_name,
                               .applicationVersion = 1,
                               .pEngineName        = ENGINE_NAME,
                               .engineVersion      = ENGINE_VERSION,
#ifdef VK_MAKE_API_VERSION
                               .apiVersion = VK_MAKE_API_VERSION(0, version_major, version_minor, 0)};
#else
                               .apiVersion = VK_MAKE_VERSION(version_major, version_minor, 0)};
#endif

    VkInstanceCreateInfo inst_info = {.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                      .pApplicationInfo        = &infos,
                                      .enabledExtensionCount   = ext_count,
                                      .ppEnabledExtensionNames = extentions,
                                      .enabledLayerCount       = enabledLayersCount,
                                      .ppEnabledLayerNames     = enabledLayers};

    //VKH_CHECK_RESULT(app->status, vkCreateInstance(&inst_info, NULL, &app->inst));
    app->status = vkCreateInstance(&inst_info, NULL, &app->inst);
    app->infos          = infos;
    app->debugMessenger = VK_NULL_HANDLE;
    return app;
}

void vkh_app_destroy(VkhApp app) {
    if (app->debugMessenger != VK_NULL_HANDLE) {
        PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessenger =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(app->inst, "vkDestroyDebugUtilsMessengerEXT");
        DestroyDebugUtilsMessenger(app->inst, app->debugMessenger, VK_NULL_HANDLE);
    }
    vkDestroyInstance(app->inst, NULL);
    free(app);
}
VkResult vkh_app_status(VkhApp app) { return app == NULL ? VK_ERROR_UNKNOWN : app->status; }

VkInstance vkh_app_get_inst(VkhApp app) { return vkh_app_status(app) ? VK_NULL_HANDLE : app->inst; }

VkhPhyInfo *vkh_app_get_phyinfos(VkhApp app, uint32_t *count, VkSurfaceKHR surface) {
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(app->inst, count, NULL));
    VkPhysicalDevice *phyDevices = (VkPhysicalDevice *)malloc((*count) * sizeof(VkPhysicalDevice));
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices(app->inst, count, phyDevices));
    VkhPhyInfo *infos = (VkhPhyInfo *)malloc((*count) * sizeof(VkhPhyInfo));

    for (uint32_t i = 0; i < (*count); i++)
        infos[i] = vkh_phyinfo_create(phyDevices[i], surface);

    free(phyDevices);
    return infos;
}

void vkh_app_free_phyinfos(uint32_t count, VkhPhyInfo *infos) {
    for (uint32_t i = 0; i < count; i++)
        vkh_phyinfo_destroy(infos[i]);
    free(infos);
}

void vkh_app_enable_debug_messenger(VkhApp app, VkDebugUtilsMessageTypeFlagsEXT typeFlags,
                                    VkDebugUtilsMessageSeverityFlagsEXT  severityFlags,
                                    PFN_vkDebugUtilsMessengerCallbackEXT callback) {

    VkDebugUtilsMessengerCreateInfoEXT info = {.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                                               .pNext = VK_NULL_HANDLE,
                                               .flags = 0,
                                               .messageSeverity = severityFlags,
                                               .messageType     = typeFlags,
                                               .pUserData       = NULL};
    if (callback == NULL)
        info.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)debugUtilsMessengerCallback;
    else
        info.pfnUserCallback = callback;

    PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessenger =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(app->inst, "vkCreateDebugUtilsMessengerEXT");

    CreateDebugUtilsMessenger(app->inst, &info, VK_NULL_HANDLE, &app->debugMessenger);
}

/*VkPhysicalDevice vkh_app_select_phy (VkhApp app, VkPhysicalDeviceType preferedPhyType) {
    if (app->phyCount == 1)
        return app->phyDevices[0];

    for (int i=0; i<app->phyCount; i++){
        VkPhysicalDeviceProperties phy;
        vkGetPhysicalDeviceProperties (app->phyDevices[i], &phy);
        if (phy.deviceType & preferedPhyType){
#if DEBUG
            printf ("GPU: %s  vulkan:%d.%d.%d driver:%d\n", phy.deviceName,
                    phy.apiVersion>>22, phy.apiVersion>>12&2048, phy.apiVersion&8191,
                    phy.driverVersion);
#endif
            return app->phyDevices[i];
        }
    }
    fprintf (stderr, "No suitable GPU found\n");
    exit (-1);
}*/
