#ifndef VKH_APP_H
#define VKH_APP_H

#include "vkh.h"

typedef struct _vkh_app_t{
    VkApplicationInfo   infos;
    VkInstance          inst;
    uint32_t            phyCount;
}vkh_app_t;
#endif
