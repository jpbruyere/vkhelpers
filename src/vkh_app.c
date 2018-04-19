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
#include "vkh_app.h"

#define ENGINE_NAME     "vkheplers"
#define ENGINE_VERSION  1

VkhApp vkh_app_create (const char* app_name, int ext_count, const char* extentions[]) {
    VkhApp app = (VkhApp)malloc(sizeof(vkh_app_t));

    VkApplicationInfo infos = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                .pApplicationName = app_name,
                                .applicationVersion = 1,
                                .pEngineName = ENGINE_NAME,
                                .engineVersion = ENGINE_VERSION,
                                .apiVersion = VK_API_VERSION_1_0};
#if DEBUG
    const uint32_t enabledLayersCount = 1;
    //const char* enabledLayers[] = {"VK_LAYER_LUNARG_core_validation"};
    const char* enabledLayers[] = {"VK_LAYER_LUNARG_standard_validation"};
#else
    const uint32_t enabledLayersCount = 0;
    const char* enabledLayers[] = NULL;
#endif

    VkInstanceCreateInfo inst_info = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                       .pApplicationInfo = &infos,
                                       .enabledExtensionCount = ext_count,
                                       .ppEnabledExtensionNames = extentions,
                                       .enabledLayerCount = enabledLayersCount,
                                       .ppEnabledLayerNames = enabledLayers };

    VK_CHECK_RESULT(vkCreateInstance (&inst_info, NULL, &app->inst));
    return app;
}

void vkh_app_destroy (VkhApp app){
    vkDestroyInstance (app->inst, NULL);
    free (app);
}

VkInstance vkh_app_get_inst (VkhApp app) {
    return app->inst;
}

VkhPhyInfo* vkh_app_get_phyinfos (VkhApp app, uint32_t* count) {
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices (app->inst, count, NULL));
    VkPhysicalDevice phyDevices[(*count)];
    VK_CHECK_RESULT(vkEnumeratePhysicalDevices (app->inst, count, phyDevices));
    VkhPhyInfo* infos = (VkhPhyInfo*)malloc((*count) * sizeof(VkhPhyInfo));

    for (int i=0; i<(*count); i++)
        infos[i] = vkh_phyinfo_create (app, phyDevices[i]);

    return infos;
}

void vkh_app_free_phyinfos (uint32_t count, VkhPhyInfo* infos) {
    for (int i=0; i<count; i++)
        vkh_phyinfo_destroy (infos[i]);
    free (infos);
}


VkPhysicalDevice vkh_app_select_phy (VkhApp app, VkPhysicalDeviceType preferedPhyType) {
    /*if (app->phyCount == 1)
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
    exit (-1);*/
}
