<h1 align="center">
  <br>
  <br>
  Vulkan Helpers
  <br>
<p align="center">
  <a href="https://www.paypal.me/GrandTetraSoftware">
	<img src="https://img.shields.io/badge/Donate-PayPal-green.svg">
  </a>
</p>
</h1>

Small personal helper library for [Vulkan](https://www.khronos.org/vulkan/) usage in **c**.
Simplify several common tasks, as image or buffers handling, swapchain initialization, etc.

**vkh**  is used by [vkvg] internally and also to speed up vulkan context creation in samples.

### Building from source

```bash
git clone https://github.com/jpbruyere/vkhelpers.git
cd vkhelpers
mkdir build
cd build
cmake ..
make && make install
```

### Adding vkh to your CMake project

- clone vkh as a subdirectory of your root dir.
- in your main CMakeFile, add `add_subdirectory (vkhelpers)`
- add to your **TARGET_INCLUDE_DIRECTORIES** `${CMAKE_CURRENT_SOURCE_DIR}/vkhelpers/include` and if you want to bypass opaque pointers and be able to address
fields of internal structures, add also `${CMAKE_CURRENT_SOURCE_DIR}/vkhelpers/src`.
- to link vkh staticaly, add to **TARGET_LINK_LIBRARIES** `vkh_static` or `vkh_shared` to link it as a shared library.

### Quick howto:

##### Create instance
```c
#include "vkh.h"
#include "vkh_phyinfo.h"

void init_vulkan () {
  const char* layers [] = {"VK_LAYER_KHRONOS_validation"};
  const char* exts [] = {"VK_EXT_debug_utils"};
	
  VkhApp app = vkh_app_create ("appname", 1, layers, 1, exts);
```
##### Select physical device

**VkhPhyInfo** is an helper structure that will store common usefull physical device informations, queues flags, memory properties in a single call for all the devices present on the machine.
```c
   VkhPhyInfo* phys = vkh_app_get_phyinfos (e->app, &phyCount, surf);
```
Once you have an array of VkhPhyInfo's, you have several functions to inspect available devices:
```c
for (uint i=0; i<phyCount; i++) {
  //check VkPhysicalDeviceProperties
  VkPhysicalDeviceProperties pdp = vkh_phyinfo_get_properties (phys[i]);
  //get VkPhysicalDeviceMemoryProperties
  VkPhysicalDeviceMemoryProperties mp = vkh_phyinfo_get_memory_properties (phys[i]);
  //get queue properties
  VkQueueFamilyProperties* vkh_phyinfo_get_queues_props(phys[i], &qCount);
}
```
VkhPhyInfo structure has the array of **VkQueueFamilyProperties** that has already been parsed two times to detect available queues types. First vkh will try to find a dedicated queue for each queue type (Graphics, Transfer, Compute) and if a type has no dedicated candidate, it will try to find a queue with the requested flag among others. Also, if you submit a valid **VkSurfaceKHR** to `vkh_app_get_phyinfos`, presentation support will be queried for all graphics queues. The result of this search may be fetched with:
```c
vkh_phyinfo_get_queue_fam_indices (phy, &presentQ, &graphQ, &transQ, &compQ);
```
vkh has one function per queue type that uses the result of this search. They may be safely called for checking queue availability, they will return false on failure. On success, the queue count of the `VkQueueFamilyProperties` of PhyInfo will be decreased.
```c
	if (vkh_phyinfo_create_presentable_queues (pi, 1, qPriorities, &pQueueInfos[qCount]))
		qCount++;
	if (vkh_phyinfo_create_compute_queues (pi, 1, qPriorities, &pQueueInfos[qCount]))
		qCount++;
	if (vkh_phyinfo_create_transfer_queues (pi, 1, qPriorities, &pQueueInfos[qCount]))
		qCount++;
```
To override the vkh default queue selection, create queues with your own family index:
```c
vkh_phyinfo_create_queues (phy, qFam, queueCount, priorities, &qInfo);
```
Be aware that the queue count of the vkhinfo structure is decreased to keep track of remaining queues.
PhyInfo pointer has to be freed when no longuer in use. Usualy at the end of the vulkan initialization.
```c
vkh_app_free_phyinfos  (phyCount, phys);
```
##### Create Logical Device

```c
char const * dex [] = {"VK_KHR_swapchain"};
VkPhysicalDeviceFeatures enabledFeatures = { .fillModeNonSolid = true };
VkDeviceCreateInfo device_info = {
  .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
  .queueCreateInfoCount = qCount,
  .pQueueCreateInfos = (VkDeviceQueueCreateInfo*)&pQueueInfos,
  .enabledExtensionCount = enabledExtsCount,
  .ppEnabledExtensionNames = dex,
  .pEnabledFeatures = &enabledFeatures};
VkhDevice dev = vkh_device_create(e->app, pi, &device_info);
```
##### The Presenter
VkhPresenter will help getting rapidly something on screen, it handles the swapchain.
```c
VkhPresenter present = vkh_presenter_create (dev, pi->pQueue, surf, width, height, VK_FORMAT_B8G8R8A8_UNORM, VK_PRESENT_MODE_MAILBOX_KHR);
//create a blitting command buffer per swapchain images with
vkh_presenter_build_blit_cmd (present, vkvg_surface_get_vk_image(surf), width, height);
while (running) {
  if (!vkh_presenter_draw (present))
    //on draw failed, swapchain is automatically rebuilt
    vkh_presenter_build_blit_cmd (present, vkvg_surface_get_vk_image(surf), width, height);
}
```
##### Creating Images
TODO
##### Creating Buffers
TODO





