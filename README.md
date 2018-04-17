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

### What is vkh?

**vkh** is a multiplatform helper library for [Vulkan](https://www.khronos.org/vulkan/) written in **c**.
vkh main goal is to offer an api which will ease the development of wrappers for higher level languages.
No additional library except vulkan is required.

### Current status:

Early development stage, api may change frequently.

### Building

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


