#ifndef VKH_QUEUE_H
#define VKH_QUEUE_H

#include "vkh.h"

typedef struct _vkh_queue_t{
    VkhDevice       dev;
    uint32_t        familyIndex;
    VkQueue         queue;
    VkQueueFlags    flags;
}vkh_queue_t;

#endif
