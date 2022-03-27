# /////////////////////////////////////////
#     VKH Library Build Configurations    #
# /////////////////////////////////////////

project(
    'vkh',
    'c', 'cpp',
    version : '0.3.0',
    meson_version : '>=0.62.0',
    default_options : [
        'c_std=c11',
        'cpp_std=c++11'
    ]
)

lib_so_version = '1'

# Enable Vulkan Validation Layer
ENABLE_VALIDATION_OPT = get_option('ENABLE_VALIDATION_OPT')

# Enable Vulkan Memory Allocator - For more information: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
# Currently the only option
VKH_ENABLE_VMA = get_option('VKH_ENABLE_VMA')

# VMA Options

# Enable VMA memory recording for debugging
VMA_RECORDING_ENABLED = get_option('VMA_RECORDING_ENABLED')

# Use C++ STL containers instead of VMA's containers
VMA_USE_STL_CONTAINERS = get_option('VMA_USE_STL_CONTAINERS')

# Link statically with Vulkan API
VMA_STATIC_VULKAN_FUNCTIONS = get_option('VMA_STATIC_VULKAN_FUNCTIONS')

# Fetch pointers to Vulkan functions internally (no static linking)
VMA_DYNAMIC_VULKAN_FUNCTIONS = get_option('VMA_DYNAMIC_VULKAN_FUNCTIONS')

# Every allocation will have its own memory block
VMA_DEBUG_ALWAYS_DEDICATED_MEMORY = get_option('VMA_DEBUG_ALWAYS_DEDICATED_MEMORY')

# Automatically fill new allocations and destroyed allocations with some bit pattern
VMA_DEBUG_INITIALIZE_ALLOCATIONS = get_option('VMA_DEBUG_INITIALIZE_ALLOCATIONS')

# Enable single mutex protecting all entry calls to the library
VMA_DEBUG_GLOBAL_MUTEX = get_option('VMA_DEBUG_GLOBAL_MUTEX')

# Never exceed VkPhysicalDeviceLimits::maxMemoryAllocationCount and return error
VMA_DEBUG_DONT_EXCEED_MAX_MEMORY_ALLOCATION_COUNT = get_option('VMA_DEBUG_DONT_EXCEED_MAX_MEMORY_ALLOCATION_COUNT')

# Minimum alignment of all allocations, in bytes.
# Set to more than 1 for debugging purposes only. Must be power of two.
VMA_DEBUG_ALIGNMENT = get_option('VMA_DEBUG_ALIGNMENT')

# Minimum margin before and after every allocation, in bytes.
# Set nonzero for debugging purposes only.
VMA_DEBUG_MARGIN = get_option('VMA_DEBUG_MARGIN')

# Minimum value for VkPhysicalDeviceLimits::bufferImageGranularity.
# Set to more than 1 for debugging purposes only. Must be power of two.
VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY = get_option('VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY')

# Maximum size of a memory heap in Vulkan to consider it "small".
VMA_SMALL_HEAP_MAX_SIZE = get_option('VMA_SMALL_HEAP_MAX_SIZE') # 1 Gigabyte default

# Default size of a block allocated as single VkDeviceMemory from a "large" heap.
VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE = get_option('VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE') # 256 Megabytes default

# Configuration for Debug/Release builds
vkh_compile_options = []

if (get_option('buildtype').startswith('debug'))
    vkh_compile_options = ['-DDEBUG']
    ENABLE_VALIDATION_OPT = true
    if (build_machine.system() == 'linux')
        vkh_compile_options += ['-Wall', '-Wno-extra', '-Wno-unknown-pragmas']
    elif (build_machine.system() == 'windows')
        vkh_compile_options += ['/W4', '/wd4204', '/wd4221', '/wd4100']
    endif
else
    if (build_machine.system() == 'linux')
        vkh_compile_options += ['-w']
    elif (build_machine.system() == 'windows')
        vkh_compile_options += ['/W0']
    endif
endif

# Enable Vulkan Memory Allocator - For more information: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
# currently the only option
if (get_option('VKH_ENABLE_VMA'))
    vkh_compile_options += '-DUSE_VMA'
endif

if (ENABLE_VALIDATION_OPT)
    vkh_compile_options += '-DVKH_USE_VALIDATION'
endif


# VMA Options Define Flags
if (VMA_RECORDING_ENABLED)
    vkh_compile_options += '-DVMA_RECORDING_ENABLED=1'
endif

if (VMA_USE_STL_CONTAINERS)
    vkh_compile_options += '-DVMA_USE_STL_CONTAINERS=1'
endif

if (VMA_STATIC_VULKAN_FUNCTIONS)
    vkh_compile_options += '-DVMA_STATIC_VULKAN_FUNCTIONS=1'
endif

if (VMA_DYNAMIC_VULKAN_FUNCTIONS)
    vkh_compile_options += '-DVMA_DYNAMIC_VULKAN_FUNCTIONS=1'
endif

if (VMA_DEBUG_ALWAYS_DEDICATED_MEMORY)
    vkh_compile_options += '-DVMA_DEBUG_ALWAYS_DEDICATED_MEMORY=1'
endif

if (VMA_DEBUG_INITIALIZE_ALLOCATIONS)
    vkh_compile_options += '-DVMA_DEBUG_INITIALIZE_ALLOCATIONS=1'
endif

if (VMA_DEBUG_GLOBAL_MUTEX)
    vkh_compile_options += '-DVMA_DEBUG_GLOBAL_MUTEX=1'
endif

if (VMA_DEBUG_DONT_EXCEED_MAX_MEMORY_ALLOCATION_COUNT)
    vkh_compile_options += '-DVMA_DEBUG_DONT_EXCEED_MAX_MEMORY_ALLOCATION_COUNT=1'
endif

if (VMA_DEBUG_ALIGNMENT != 1)
    vkh_compile_options += '-DVMA_DEBUG_ALIGNMENT=' + VMA_DEBUG_ALIGNMENT
endif

if (VMA_DEBUG_MARGIN != 0)
    vkh_compile_options += '-DVMA_DEBUG_MARGIN=' + VMA_DEBUG_MARGIN
endif

if (VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY != 1)
    vkh_compile_options += '-DVMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY=' + VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY
endif

if (VMA_SMALL_HEAP_MAX_SIZE != 1073741824) # 1 Gigabyte
    vkh_compile_options += '-DVMA_SMALL_HEAP_MAX_SIZE=' + VMA_SMALL_HEAP_MAX_SIZE
endif

if (VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE != 268435456) # 256 Megabytes
    vkh_compile_options += '-DVMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE=' + VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE
endif

# vkh dependencies
vulkan_dep = dependency('vulkan')
threads_dep = dependency('threads')
vkh_dependencies = [vulkan_dep, threads_dep]

pkg_conf = import('pkgconfig')


vkh_src = [
    'src/vkh_app.c',
    'src/vkh_buffer.c',
    'src/vkh_device.c',
    'src/vkh_image.c',
    'src/vkh_phyinfo.c',
    'src/vkh_presenter.c',
    'src/vkh_queue.c',
    'src/vkhelpers.c',
    'src/deps/tinycthread.c',
    'src/VmaUsage.cpp'
]

vkh_include = [
    'include',
    'src',
    'src/deps'
]

vkh_shared_library = shared_library('vkh', 
    c_args: [vkh_compile_options, '-DVKH_SHARED_BUILD'],
    cpp_args: [vkh_compile_options, '-DVKH_SHARED_BUILD'],
    build_by_default: true,
    install: true,
    soversion: lib_so_version,
    sources: vkh_src,
    include_directories: vkh_include,
    dependencies: vkh_dependencies
)

vkh_static_library = static_library('vkh', 
    c_args: [vkh_compile_options, '-DVKH_STATIC_BUILD'],
    cpp_args: [vkh_compile_options, '-DVKH_STATIC_BUILD'],
    build_by_default: true,
    install: true,
    sources: vkh_src,
    include_directories: vkh_include,
    dependencies: vkh_dependencies
)

install_headers('include/vkh.h', subdir: 'vkh')

vkh_shared_dep = declare_dependency(dependencies: vkh_dependencies, include_directories: 'include', link_with: vkh_shared_library)
vkh_static_dep = declare_dependency(dependencies: vkh_dependencies, include_directories: 'include', link_with: vkh_static_library)


# generate a pkgconfig file for shared lib
pkg_conf.generate(vkh_shared_library,
    name : meson.project_name(),
    version : meson.project_version(),
    filebase : meson.project_name(),
    description : 'Vulkan helpers library',
)

summary ({
    'ENABLE_VALIDATION_OPT': ENABLE_VALIDATION_OPT,
    'VKH_ENABLE_VMA': VKH_ENABLE_VMA
}, section: 'VKH Options')

summary ({
    'VMA_RECORDING_ENABLED': VMA_RECORDING_ENABLED,
    'VMA_USE_STL_CONTAINERS': VMA_USE_STL_CONTAINERS,
    'VMA_STATIC_VULKAN_FUNCTIONS': VMA_STATIC_VULKAN_FUNCTIONS,
    'VMA_DYNAMIC_VULKAN_FUNCTIONS': VMA_DYNAMIC_VULKAN_FUNCTIONS,
    'VMA_DEBUG_ALWAYS_DEDICATED_MEMORY': VMA_DEBUG_ALWAYS_DEDICATED_MEMORY,
    'VMA_DEBUG_INITIALIZE_ALLOCATIONS': VMA_DEBUG_INITIALIZE_ALLOCATIONS,
    'VMA_DEBUG_GLOBAL_MUTEX': VMA_DEBUG_GLOBAL_MUTEX,
    'VMA_DEBUG_DONT_EXCEED_MAX_MEMORY_ALLOCATION_COUNT': VMA_DEBUG_DONT_EXCEED_MAX_MEMORY_ALLOCATION_COUNT,
    'VMA_DEBUG_ALIGNMENT': VMA_DEBUG_ALIGNMENT,
    'VMA_DEBUG_MARGIN': VMA_DEBUG_MARGIN,
    'VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY': VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY,
    'VMA_SMALL_HEAP_MAX_SIZE': VMA_SMALL_HEAP_MAX_SIZE,
    'VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE': VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE,
}, section: 'VMA Options')
