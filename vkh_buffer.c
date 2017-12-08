#include "vkh_buffer.h"
#include "vkhelpers.h"

void vkh_buffer_create(vkh_device *pDev, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size, vkh_buffer* buff){
    buff->pDev = pDev;
    VkBufferCreateInfo bufCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .usage = usage, .size = size, .sharingMode = VK_SHARING_MODE_EXCLUSIVE};
    VK_CHECK_RESULT(vkCreateBuffer(pDev->vkDev, &bufCreateInfo, NULL, &buff->buffer));

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(pDev->vkDev, buff->buffer, &memReq);
    VkMemoryAllocateInfo memAllocInfo = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
                                          .allocationSize = memReq.size };
    assert(memory_type_from_properties(&pDev->phyMemProps, memReq.memoryTypeBits,memoryPropertyFlags, &memAllocInfo.memoryTypeIndex));
    VK_CHECK_RESULT(vkAllocateMemory(pDev->vkDev, &memAllocInfo, NULL, &buff->memory));

    buff->alignment = memReq.alignment;
    buff->size = memAllocInfo.allocationSize;
    buff->usageFlags = usage;
    buff->memoryPropertyFlags = memoryPropertyFlags;

    VK_CHECK_RESULT(vkh_buffer_bind(buff));
}

void vkh_buffer_destroy(vkh_buffer* buff){
    if (buff->buffer)
        vkDestroyBuffer(buff->pDev->vkDev, buff->buffer, NULL);
    if (buff->memory)
        vkFreeMemory(buff->pDev->vkDev, buff->memory, NULL);
}


VkResult vkh_buffer_map(vkh_buffer* buff){
    return vkMapMemory(buff->pDev->vkDev, buff->memory, 0, VK_WHOLE_SIZE, 0, &buff->mapped);
}
void vkh_buffer_unmap(vkh_buffer* buff){
    if (!buff->mapped)
        return;
    vkUnmapMemory(buff->pDev->vkDev, buff->memory);
    buff->mapped = NULL;
}

VkResult vkh_buffer_bind(vkh_buffer* buff)
{
    return vkBindBufferMemory(buff->pDev->vkDev, buff->buffer, buff->memory, 0);
}
