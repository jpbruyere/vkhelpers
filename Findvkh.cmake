find_path(vkh_INCLUDE_DIR vkh.h)

find_library(vkh_LIBRARY NAMES vkh)

# handle the QUIETLY and REQUIRED arguments and set VKHFOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(vkh DEFAULT_MSG
  vkh_LIBRARY  vkh_INCLUDE_DIR)

if(vkh_FOUND)
  set( vkh_LIBRARIES ${vkh_LIBRARY} )
endif()

mark_as_advanced(vkh_INCLUDE_DIR vkh_LIBRARY vkh_LIBRARIES)
