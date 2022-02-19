project(
    'vkh',
    'c', 'cpp',
    version : '0.1.0',
    meson_version : '>=0.56.2',
    default_options : [
        'c_std=c11',
        'cpp_std=c++11',
        'libdir=lib' #default in cmake
    ]
)

lib_so_version = '1'

# meson defaults to a debug build
if get_option('buildtype').startswith('debug')
    add_project_arguments(['-DDEBUG', '-Wall', 
        '-Wno-extra', '-Wno-unknown-pragmas'], language : 'c' )
else
    add_project_arguments('-w', language : 'c')
endif


if get_option('VKH_ENABLE_VMA') # currently the only option and is required
    add_project_arguments('-DUSE_VMA', language : 'cpp')
endif

if get_option('ENABLE_VALIDATION')
    add_project_arguments('-DVKH_USE_VALIDATION', language : 'c')
endif

vulkan_dep = dependency('vulkan')

pkg_conf = import('pkgconfig')

subdir('include')

subdir('src')

declare_dependency(
    link_with : vk_helper_lib,
    include_directories : vk_helpers_inc
)

pkg_conf.generate(vk_helper_lib,
    name : meson.project_name(),
    version : meson.project_version(),
    filebase : meson.project_name(),
    description : 'Vulkan helpers library',
)