# - Try to find FFMPEG
# Once done this will define
#  
#  FFMPEG_FOUND		   - system has FFMPEG
#  FFMPEG_INCLUDE_DIRS	   - the include directories
#  FFMPEG_LIBRARIES        - dynamic FFMPEG libraries
#  FFMPEG_STATIC_LIBRARIES - static FFMPEG libraries
#
# You can specify the following components:
#  * avcodec
#  * avformat
#  * avutil
#  * avdevice
#  * swscale
#  * postproc
#
# This will define:
#  FFMPEG_<component>_FOUND	     - FFMPEG also has <component>
#  FFMPEG_<component>_INCLUDE_DIR    - include directoty of <component>
#  FFMPEG_<component>_LIBRARY        - dynamic library of <component>
#  FFMPEG_<component>_STATIC_LIBRARY - static library of <component>
#
# Copyright (c)2012, Dirk Van Haerenborgh
#


include(LibFindMacros)


macro(MACRO_FIND_FFMPEG_COMPONENT _component)
    string(TOUPPER ${_component} _upper)

    find_library(FFMPEG_${_upper}_LIBRARY
        NAMES ${CMAKE_SHARED_LIBRARY_PREFIX}${_component}${CMAKE_SHARED_LIBRARY_SUFFIX}
        HINTS "$ENV{FFMPEGDIR}/lib"  "${FFMPEGDIR}/lib"
    )

    find_library(FFMPEG_${_upper}_STATIC_LIBRARY
        NAMES ${CMAKE_STATIC_LIBRARY_PREFIX}${_component}${CMAKE_STATIC_LIBRARY_SUFFIX}
        HINTS "$ENV{FFMPEGDIR}/lib"  "${FFMPEGDIR}/lib"
    )

    find_path(FFMPEG_${_upper}_INCLUDE_DIR
        NAMES "lib${_component}/${_component}.h"
        HINTS "$ENV{FFMPEGDIR}/include"  "${FFMPEGDIR}/include"
    )

    if(FFMPEG_${_upper}_INCLUDE_DIR)
        set(FFMPEG_${_upper}_FOUND TRUE)
        list(APPEND FFMPEG_INCLUDE_DIRS ${FFMPEG_${_upper}_INCLUDE_DIR})

        if(FFMPEG_${_upper}_LIBRARY)
            list(APPEND FFMPEG_LIBRARIES ${FFMPEG_${_upper}_LIBRARY})
        else()
            message(STATUS "Could not find ${_component} shared library")
            set(FFMPEG_${_upper}_LIBRARY FFMPEG_${_upper}_LIBRARY-NOTFOUND)
        endif()

        if(FFMPEG_${_upper}_STATIC_LIBRARY)
            list(APPEND FFMPEG_STATIC_LIBRARIES ${FFMPEG_${_upper}_STATIC_LIBRARY})
        else()
            message(STATUS "Could not find ${_component} static library")
            set(FFMPEG_${_upper}_STATIC_LIBRARY FFMPEG_${_upper}_STATIC_LIBRARY-NOTFOUND)

        endif()

    else()
        message(STATUS "Could not find ${_component}")
        set(FFMPEG_${_upper}_INCLUDE_DIR FFMPEG_${_upper}_INCLUDE_DIR-NOTFOUND)
        set(FFMPEG_${_upper}_LIBRARY FFMPEG_${_upper}_LIBRARY-NOTFOUND)
        set(FFMPEG_${_upper}_STATIC_LIBRARY FFMPEG_${_upper}_STATIC_LIBRARY-NOTFOUND)
    endif()

   mark_as_advanced(FFMPEG_${_upper}_LIBRARY FFMPEG_${_upper}_STATIC_LIBRARY FFMPEG_${_upper}_INCLUDE_DIR)

endmacro(MACRO_FIND_FFMPEG_COMPONENT)



# List of all gstreamer plugins base components
# The gstinterfaces library is found by FindGStreamer.cmake
set(FFMPEG_ALL_COMPONENTS
    avcodec
    avformat
    avutil
    avdevice
    swscale
    postproc
)


# If the user specified required components e.g. via
# FIND_PACKAGE(GstreamerPluginsBase REQUIRED audio video)
# find (just) those libraries. Otherwise find all libraries.
if(FFMPEG_FIND_COMPONENTS)
    set(_FFMPEG_FIND_COMPONENTS ${FFMPEG_FIND_COMPONENTS})
else(FFMPEG_FIND_COMPONENTS)
    set(_FFMPEG_FIND_COMPONENTS ${FFMPEG_ALL_COMPONENTS})
endif(FFMPEG_FIND_COMPONENTS)


foreach(component ${_FFMPEG_FIND_COMPONENTS})
  macro_find_ffmpeg_component(${component})
endforeach(component)

list(REMOVE_DUPLICATES FFMPEG_LIBRARIES)
list(REMOVE_DUPLICATES FFMPEG_STATIC_LIBRARIES)
list(REMOVE_DUPLICATES FFMPEG_INCLUDE_DIRS)
if(NOT FFMPEG_FIND_QUIETLY)

    set(PLUGINS_FOUND "")
    foreach(component ${_FFMPEG_FIND_COMPONENTS})
        string(TOUPPER ${component} _upper)
        if( FFMPEG_${_upper}_LIBRARY )
            set(PLUGINS_FOUND ${PLUGINS_FOUND} "\n\t" ${component})
        endif()
    endforeach(component)

    #remove empty lines
    string(REGEX REPLACE "\n[ ]*(\n[ ]*)*" "\n"  PLUGINS_FOUND ${PLUGINS_FOUND})
    message(STATUS "Found FFMPEG components: ${PLUGINS_FOUND}")

endif(NOT FFMPEG_FIND_QUIETLY)

