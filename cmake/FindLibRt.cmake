# Locate librt library
# This module defines
#  LIBRT_FOUND - System has librt
#  LIBRT_LIBRARIES - The librt library
# https://github.com/rlsosborne/tool_axe/blob/master/cmake/Modules/FindLibRt.cmake

find_library(LIBRT_LIBRARIES rt)

include(FindPackageHandleStandardArgs)
# Sets LIBRT_FOUND
find_package_handle_standard_args(LibRt DEFAULT_MSG LIBRT_LIBRARIES)

mark_as_advanced(LIBRT_LIBRARIES)
