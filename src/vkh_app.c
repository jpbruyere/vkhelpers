#include "vkh_app.h"

#define ENGINE_NAME     "vkheplers"
#define ENGINE_VERSION  1

VkhApp vkh_app_create (const char* app_name, const char* extentions[], int ext_count) {
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

VkPhysicalDevice vkh_app_select_phy (VkhApp app, VkPhysicalDeviceType preferedPhyType) {
    uint32_t gpu_count = 0;

    VK_CHECK_RESULT(vkEnumeratePhysicalDevices (app->inst, &gpu_count, NULL));

    VkPhysicalDevice phys[gpu_count];

    VK_CHECK_RESULT(vkEnumeratePhysicalDevices (app->inst, &gpu_count, &phys));

    if (gpu_count == 1)
        return phys[0];

    for (int i=0; i<gpu_count; i++){
        VkPhysicalDeviceProperties phy;
        vkGetPhysicalDeviceProperties (phys[i], &phy);
        if (phy.deviceType & preferedPhyType){
            printf ("GPU: %s  vulkan:%d.%d.%d driver:%d\n", phy.deviceName,
                    phy.apiVersion>>22, phy.apiVersion>>12&2048, phy.apiVersion&8191,
                    phy.driverVersion);
            return phys[i];
        }
    }
    fprintf (stderr, "No suitable GPU found\n");
    exit (-1);
}
