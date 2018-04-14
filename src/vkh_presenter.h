#ifndef VKH_PRESENTER_H
#define VKH_PRESENTER_H

#include "vkh.h"

typedef struct ImageBuffer_t {
    VkImage     image;
    VkImageView view;
}ImageBuffer;

typedef struct _vkh_presenter_t {
    VkQueue         queue;
    VkCommandPool   cmdPool;
    uint32_t        qFam;
    VkDevice        dev;

    //GLFWwindow*     window;
    VkSurfaceKHR    surface;

    VkSemaphore     semaPresentEnd;
    VkSemaphore     semaDrawEnd;

    VkFormat        format;
    VkColorSpaceKHR colorSpace;
    VkPresentModeKHR presentMode;
    uint32_t        width;
    uint32_t        height;

    uint32_t        imgCount;
    uint32_t        currentScBufferIndex;

    VkRenderPass    renderPass;
    VkSwapchainKHR  swapChain;
    ImageBuffer*    ScBuffers;
    VkCommandBuffer* cmdBuffs;
    VkFramebuffer*  frameBuffs;
}vkh_presenter_t;

#endif
