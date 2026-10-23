/*
In exactly one CPP file define macro VMA_IMPLEMENTATION and then include
vk_mem_alloc.h to include definitions of its internal implementation
*/
#pragma warning(push)
#pragma warning(disable : 4100; disable : 4127; disable : 4324)

//#define VMA_DEBUG_LOG(format, ...) printf(format "\n", ##__VA_ARGS__)
// Alternatively, tell VMA how to print leaks explicitly
//#define VMA_LEAK_LOG_FORMAT(format) "VMA LEAK: " format

#define VMA_IMPLEMENTATION
#include <cstdio>
#include "../vk_mem_alloc.h"
#pragma warning(pop)
