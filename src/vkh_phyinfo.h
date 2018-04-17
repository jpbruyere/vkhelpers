#ifndef VKH_PHY_H
#define VKH_PHY_H

#include "vkh.h"

typedef struct _vkh_phy_t{
    VkPhysicalDevice                    phy;
    VkPhysicalDeviceMemoryProperties    memProps;
    VkPhysicalDeviceProperties          properties;
    VkQueueFamilyProperties*            queues;
    uint32_t                            queueCount;
    int                                 cQueue;
    int                                 gQueue;
    int                                 tQueue;

    uint32_t                            qCreateInfosCount;
    VkDeviceQueueCreateInfo*            qCreateInfos;
}vkh_phy_t;
#endif
