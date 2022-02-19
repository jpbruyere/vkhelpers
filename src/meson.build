vkhelpers_sources = [
    'vkh_app.c', 'vkh_buffer.c',
    'vkh_device.c', 'vkh_image.c',
    'vkh_phyinfo.c', 'vkh_presenter.c',
    'vkh_queue.c', 'vkhelpers.c',
    'VmaUsage.cpp'
]

vk_helper_lib = library(meson.project_name(),
    vkhelpers_sources,
    include_directories : [vk_helpers_inc],
    dependencies : vulkan_dep,
    version : meson.project_version(),
    soversion : lib_so_version,
    install : true
)

