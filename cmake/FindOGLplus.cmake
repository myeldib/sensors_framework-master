find_path(OGLplus_INCLUDE_DIR
          NAMES oglplus/gl.hpp
          PATHS ${OGLplus_INCLUDE_DIR}
                $ENV{OGLplus_INCLUDE_DIR})

set(OGLplus_INCLUDE_DIRS ${OGLplus_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(OGLplus
                                  DEFAULT_MSG
                                  OGLplus_INCLUDE_DIR)

mark_as_advanced(OGLplus_INCLUDE_DIR)
