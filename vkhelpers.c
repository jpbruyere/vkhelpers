
#include "vkhelpers.h"

VkPhysicalDevice vkh_find_phy (VkInstance inst, VkPhysicalDeviceType phyType) {
    uint32_t gpu_count = 0;

    VK_CHECK_RESULT(vkEnumeratePhysicalDevices (inst, &gpu_count, NULL));

    VkPhysicalDevice phys[gpu_count];

    VK_CHECK_RESULT(vkEnumeratePhysicalDevices (inst, &gpu_count, &phys));

    for (int i=0; i<gpu_count; i++){
        VkPhysicalDeviceProperties phy;
        vkGetPhysicalDeviceProperties (phys[i], &phy);
        if (phy.deviceType & phyType){
            printf ("GPU: %s  vulkan:%d.%d.%d driver:%d\n", phy.deviceName,
                    phy.apiVersion>>22, phy.apiVersion>>12&2048, phy.apiVersion&8191,
                    phy.driverVersion);
            return phys[i];
        }
    }
    fprintf (stderr, "No suitable GPU found\n");
    exit (-1);
}

VkFence vkh_fence_create (VkDevice dev) {
    VkFence fence;
    VkFenceCreateInfo fenceInfo = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
                                    .pNext = NULL,
                                    .flags = 0 };
    VK_CHECK_RESULT(vkCreateFence(dev, &fenceInfo, NULL, &fence));
    return fence;
}
VkSemaphore vkh_semaphore_create (VkDevice dev) {
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo info = { .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
                                                           .pNext = NULL,
                                                           .flags = 0};
    VK_CHECK_RESULT(vkCreateSemaphore(dev, &info, NULL, &semaphore));
    return semaphore;
}
VkCommandPool vkh_cmd_pool_create (VkDevice dev, uint32_t qFamIndex, VkCommandPoolCreateFlags flags){
    VkCommandPool cmdPool;
    VkCommandPoolCreateInfo cmd_pool_info = { .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
                                              .pNext = NULL,
                                              .queueFamilyIndex = qFamIndex,
                                              .flags = flags };
    VK_CHECK_RESULT (vkCreateCommandPool(dev, &cmd_pool_info, NULL, &cmdPool));
    return cmdPool;
}
VkCommandBuffer vkh_cmd_buff_create (VkDevice dev, VkCommandPool cmdPool, VkCommandBufferLevel level){
    VkCommandBuffer cmdBuff;
    VkCommandBufferAllocateInfo cmd = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                        .pNext = NULL,
                                        .commandPool = cmdPool,
                                        .level = level,
                                        .commandBufferCount = 1 };
    VK_CHECK_RESULT (vkAllocateCommandBuffers (dev, &cmd, &cmdBuff));
    return cmdBuff;
}

void vkh_cmd_begin(VkCommandBuffer cmdBuff, VkCommandBufferUsageFlags flags) {
    VkCommandBufferBeginInfo cmd_buf_info = { .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
                                              .pNext = NULL,
                                              .flags = flags,
                                              .pInheritanceInfo = NULL };

    VK_CHECK_RESULT (vkBeginCommandBuffer (cmdBuff, &cmd_buf_info));
}
void vkh_cmd_end(VkCommandBuffer cmdBuff){
    VK_CHECK_RESULT (vkEndCommandBuffer (cmdBuff));
}
void vkh_cmd_submit(VkQueue queue, VkCommandBuffer *pCmdBuff, VkFence fence){
    VkSubmitInfo submit_info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                 .commandBufferCount = 1,
                                 .pCommandBuffers = pCmdBuff};
    VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, fence));
}
void vkh_cmd_submit_with_semaphores(VkQueue queue, VkCommandBuffer *pCmdBuff, VkSemaphore waitSemaphore,
                                    VkSemaphore signalSemaphore, VkFence fence){

    VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    VkSubmitInfo submit_info = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                 .pWaitDstStageMask = &stageFlags,
                                 .commandBufferCount = 1,
                                 .pCommandBuffers = pCmdBuff};

    if (waitSemaphore != VK_NULL_HANDLE){
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &waitSemaphore;
    }
    if (signalSemaphore != VK_NULL_HANDLE){
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores= &signalSemaphore;
    }

    VK_CHECK_RESULT(vkQueueSubmit(queue, 1, &submit_info, fence));
}



void set_image_layout(VkCommandBuffer cmdBuff, VkImage image, VkImageAspectFlags aspectMask, VkImageLayout old_image_layout,
                      VkImageLayout new_image_layout, VkPipelineStageFlags src_stages, VkPipelineStageFlags dest_stages) {
    VkImageMemoryBarrier image_memory_barrier = { .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                                                  .oldLayout = old_image_layout,
                                                  .newLayout = new_image_layout,
                                                  .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                  .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                                                  .image = image,
                                                  .subresourceRange = {aspectMask,0,1,0,1}};

    switch (old_image_layout) {
        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_PREINITIALIZED:
            image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
            break;

        default:
            break;
    }

    switch (new_image_layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            break;

        case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            break;

        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            image_memory_barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            break;

        default:
            break;
    }

    vkCmdPipelineBarrier(cmdBuff, src_stages, dest_stages, 0, 0, NULL, 0, NULL, 1, &image_memory_barrier);
}

bool memory_type_from_properties(VkPhysicalDeviceMemoryProperties* memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t *typeIndex) {
    // Search memtypes to find first index with those properties
    for (uint32_t i = 0; i < memory_properties->memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            // Type is available, does it match user properties?
            if ((memory_properties->memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask) {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure
    return false;
}

VkShaderModule vkh_load_module(VkDevice dev, const char* path){
    VkShaderModule module;
    size_t filelength;
    char* pCode = read_spv(path, &filelength);
    VkShaderModuleCreateInfo createInfo = { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                                            .pCode = pCode,
                                            .codeSize = filelength };
    VK_CHECK_RESULT(vkCreateShaderModule(dev, &createInfo, NULL, &module));
    free (pCode);
    //assert(module != VK_NULL_HANDLE);
    return module;
}

char *read_spv(const char *filename, size_t *psize) {
    long int size;
    size_t retval;
    void *shader_code;

#if (defined(VK_USE_PLATFORM_IOS_MVK) || defined(VK_USE_PLATFORM_MACOS_MVK))
    filename =[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: @(filename)].UTF8String;
#endif

    FILE *fp = fopen(filename, "rb");
    if (!fp)
        return NULL;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);

    shader_code = malloc(size);
    retval = fread(shader_code, size, 1, fp);
    assert(retval == 1);

    *psize = size;

    fclose(fp);
    return shader_code;
}

// Read file into array of bytes, and cast to uint32_t*, then return.
// The data has been padded, so that it fits into an array uint32_t.
uint32_t* readFile(uint32_t* length, const char* filename) {

    FILE* fp = fopen(filename, "rb");
    if (fp == 0) {
        printf("Could not find or open file: %s\n", filename);
    }

    // get file size.
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    long filesizepadded = (long)(ceil(filesize / 4.0)) * 4;

    // read file contents.
    char *str = (char*)malloc(filesizepadded*sizeof(char));
    fread(str, filesize, sizeof(char), fp);
    fclose(fp);

    // data padding.
    for (int i = filesize; i < filesizepadded; i++)
        str[i] = 0;

    *length = filesizepadded;
    return (uint32_t *)str;
}

void dumpLayerExts () {
    printf ("Layers:\n");
    uint32_t instance_layer_count;
    assert (vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL)==VK_SUCCESS);
    if (instance_layer_count == 0)
        return;
    VkLayerProperties vk_props[instance_layer_count];
    assert (vkEnumerateInstanceLayerProperties(&instance_layer_count, vk_props)==VK_SUCCESS);

    for (uint32_t i = 0; i < instance_layer_count; i++) {
        printf ("\t%s, %s\n", vk_props[i].layerName, vk_props[i].description);
/*        res = init_global_extension_properties(layer_props);
        if (res) return res;
        info.instance_layer_properties.push_back(layer_props);*/
    }
}
