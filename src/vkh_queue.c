#include "vkh_queue.h"
#include "vkh_device.h"

VkhQueue _init_queue (VkhDevice dev) {
    VkhQueue q  = (vkh_queue_t*)calloc(1, sizeof(vkh_queue_t));
    q->dev = dev;
    return q;
}


VkhQueue vkh_queue_create (VkhDevice dev, uint32_t familyIndex, uint32_t qIndex, VkQueueFlags flags) {
    VkhQueue q  = _init_queue (dev);
    q->familyIndex  = familyIndex;
    vkGetDeviceQueue (dev->dev, familyIndex, qIndex, &q->queue);
    return q;
}

VkhQueue vkh_queue_find (VkhDevice dev, VkQueueFlags flags) {
    uint32_t qFamCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties (dev->phy, &qFamCount, NULL);
    VkQueueFamilyProperties qFams[qFamCount];
    vkGetPhysicalDeviceQueueFamilyProperties (dev->phy, &qFamCount, qFams);

    //first try to find dedicated queue
    for (int i=0; i<qFamCount; i++){
        if (qFams[i].queueFlags == flags)
            return vkh_queue_create (dev, i, 0, qFams[i].queueFlags);
    }
    //if not found, get matching q
    for (int i=0; i<qFamCount; i++){
        if ((qFams[i].queueFlags & flags) == flags)
            return vkh_queue_create (dev, i, 0, qFams[i].queueFlags);
    }
    return VK_NULL_HANDLE;
}

void vkh_queue_destroy (VkhQueue queue){
    free (queue);
}
