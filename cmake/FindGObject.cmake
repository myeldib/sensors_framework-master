# - Try to find GObject
# Once done this will define
#
#  GOBJECT_FOUND - system has GObject
#  GOBJECT_INCLUDE_DIR - the GObject include directory
#  GOBJECT_LIBRARIES - the libraries needed to use GObject
#  GOBJECT_DEFINITIONS - Compiler switches required for using GObject

# Copyright (c) 2011, Raphael Kubo da Costa <kubito@gmail.com>
# Copyright (c) 2006, Tim Beaulen <tbscope@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if(NOT "${CMAKE_PREFIX_PATH}" STREQUAL "")
        set(ENV{PKG_CONFIG_PATH} "${CMAKE_PREFIX_PATH}/lib/pkgconfig:${CMAKE_PREFIX_PATH}/lib64/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif(NOT "${CMAKE_PREFIX_PATH}" STREQUAL "")

if(NOT "${GOBJECT_ROOT}" STREQUAL "")
        set(ENV{PKG_CONFIG_PATH} "${GOBJECT_ROOT}/lib/pkgconfig:${GOBJECT_ROOT}/lib64/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif(NOT "${GOBJECT_ROOT}" STREQUAL "")



find_package(PkgConfig)
pkg_check_modules(PC_GOBJECT gobject-2.0)
set(GOBJECT_DEFINITIONS ${PC_GOBJECT_CFLAGS_OTHER})

find_path(GOBJECT_INCLUDE_DIR gobject.h
   HINTS
   ${PC_GOBJECT_INCLUDEDIR}
   ${PC_GOBJECT_INCLUDE_DIRS}
   PATH_SUFFIXES glib-2.0/gobject/
   )

find_library(_GObjectLibs NAMES gobject-2.0
   HINTS
   ${PC_GOBJECT_LIBDIR}
   ${PC_GOBJECT_LIBRARY_DIRS}
   )
find_library(_GModuleLibs NAMES gmodule-2.0
   HINTS
   ${PC_GOBJECT_LIBDIR}
   ${PC_GOBJECT_LIBRARY_DIRS}
   )
find_library(_GThreadLibs NAMES gthread-2.0
   HINTS
   ${PC_GOBJECT_LIBDIR}
   ${PC_GOBJECT_LIBRARY_DIRS}
   )
find_library(_GLibs NAMES glib-2.0
   HINTS
   ${PC_GOBJECT_LIBDIR}
   ${PC_GOBJECT_LIBRARY_DIRS}
   )

set( GOBJECT_LIBRARIES ${_GObjectLibs} ${_GModuleLibs} ${_GThreadLibs} ${_GLibs} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GOBJECT DEFAULT_MSG GOBJECT_LIBRARIES GOBJECT_INCLUDE_DIR)

mark_as_advanced(GOBJECT_INCLUDE_DIR _GObjectLibs _GModuleLibs _GThreadLibs _GLibs)
