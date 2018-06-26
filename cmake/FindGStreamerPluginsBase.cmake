# - Try to find GStreamer_Plugins
# Once done this will define
#
#  GSTREAMERPLUGINSBASE_FOUND - system has GStreamer_Plugins
#  GSTREAMERPLUGINSBASE_INCLUDE_DIRS - the GStreamer_Plugins include directories
#  GSTREAMERPLUGINSBASE_LIBRARIES - the libraries needed to use GStreamer_Plugins
#
# The following variables are set for each plugin PLUGINNAME:
#
#  GSTREAMER_PLUGIN_PLUGINNAME_FOUND - plugin is found
#  GSTREAMER_PLUGIN_PLUGINNAME_INCLUDE_DIR - plugin include directory
#  GSTREAMER_PLUGIN_PLUGINNAME_LIBRARY - the library needed to use plugin
#
#  (c)2009      Nokia Corporation
#  (c)2010      Yury G. Kudryashov <urkud@ya.ru>
#  (c)2012-2013 Dirk Van Haerenborgh



include(LibFindMacros)

if(NOT "${CMAKE_PREFIX_PATH}" STREQUAL "")
    set(ENV{PKG_CONFIG_PATH} "${CMAKE_PREFIX_PATH}/lib/pkgconfig:${CMAKE_PREFIX_PATH}/lib64/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif(NOT "${CMAKE_PREFIX_PATH}" STREQUAL "")

if(NOT "${GSTREAMER_PLUGINSBASE_ROOT}" STREQUAL "")
    set(ENV{PKG_CONFIG_PATH} "${GSTREAMER_PLUGINSBASE_ROOT}/lib/pkgconfig:${GSTREAMER_PLUGINSBASE_ROOT}/lib64/pkgconfig:$ENV{PKG_CONFIG_PATH}")
endif(NOT "${GSTREAMER_PLUGINSBASE_ROOT}" STREQUAL "")

find_package(PkgConfig REQUIRED)

if(NOT DEFINED GStreamerPluginsBase_FIND_VERSION)
    set(GStreamerPluginsBase_FIND_VERSION "0.10")
endif(NOT DEFINED GStreamerPluginsBase_FIND_VERSION)

set(GstPluginsBase_libversion "${GStreamerPluginsBase_FIND_VERSION_MAJOR}.${GStreamerPluginsBase_FIND_VERSION_MINOR}")

set(GSTREAMERPLUGINSBASE_INCLUDE_DIRS "")
set(GSTREAMERPLUGINSBASE_LIBRARIES "")


if(NOT WIN32)
   # don't make this check required - otherwise you can't use macro_optional_find_package on this one
   pkg_check_modules( PKG_GSTREAMER_PLUGINSBASE gstreamer-plugins-base-${GstPluginsBase_libversion} )
   set(GStreamerPluginsBase_FOUND ${PKG_GSTREAMER_PLUGINSBASE_FOUND})
endif(NOT WIN32)

macro(MACRO_FIND_GSTREAMER_PLUGIN _plugin _header)
   string(TOUPPER ${_plugin} _upper)
   if(NOT WIN32)
      # don't make this check required - otherwise you can't use macro_optional_find_package on this one
      pkg_check_modules( PKG_GSTREAMER_${_upper} gstreamer-${_plugin}-${GstPluginsBase_libversion} )
   endif(NOT WIN32)

   find_library(GSTREAMER_PLUGIN_${_upper}_LIBRARY NAMES gst${_plugin}-${GstPluginsBase_libversion}
      HINTS
      ${PKG_GSTREAMER_PLUGINSBASE_LIBRARY_DIRS}
      ${PKG_GSTREAMER_${_upper}_LIBRARY_DIRS}
      )

   find_path(GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR
       NAMES gst/${_plugin}/${_header}
       HINTS
       ${PKG_GSTREAMER_PLUGINSBASE_INCLUDE_DIRS}
       ${PKG_GSTREAMER_${_upper}_INCLUDE_DIRS}
       )

   if(GSTREAMER_PLUGIN_${_upper}_LIBRARY AND GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR)
      set(GSTREAMER_PLUGIN_${_upper}_FOUND TRUE)
      list(APPEND GSTREAMERPLUGINSBASE_INCLUDE_DIRS ${GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR})
      list(APPEND GSTREAMERPLUGINSBASE_LIBRARIES ${GSTREAMER_PLUGIN_${_upper}_LIBRARY})
   else(GSTREAMER_PLUGIN_${_upper}_LIBRARY AND GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR)
      message(STATUS "Could not find ${_plugin} plugin")
      message(STATUS "${_upper} library: ${GSTREAMER_PLUGIN_${_upper}_LIBRARY}")
      message(STATUS "${_upper} include dir: ${GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR}")
      set(GSTREAMER_PLUGIN_${_upper}_FOUND FALSE)
      set(GSTREAMER_PLUGIN_${_upper}_LIBRARY GSTREAMER_PLUGIN_${_upper}_LIBRARY-NOTFOUND)
      set(GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR-NOTFOUND)
#     set(GStreamerPluginsBase_FOUND FALSE)
   endif(GSTREAMER_PLUGIN_${_upper}_LIBRARY AND GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR)

   mark_as_advanced(GSTREAMER_PLUGIN_${_upper}_LIBRARY GSTREAMER_PLUGIN_${_upper}_INCLUDE_DIR)
   
endmacro(MACRO_FIND_GSTREAMER_PLUGIN)


# List of all gstreamer plugins base components
# The gstinterfaces library is found by FindGStreamer.cmake
set(GSTREAMER_PLUGINSBASE_ALL_COMPONENTS
    audio
    cdda
    netbuffer
    pbutils
    riff
    rtp
    rtsp
    sdp
    tag
    video
)

set(GSTREAMER_PLUGINSBASE_audio_HEADER "audio.h")
set(GSTREAMER_PLUGINSBASE_video_HEADER "video.h")
set(GSTREAMER_PLUGINSBASE_cdda_HEADER "gstcddabasesrc.h")
set(GSTREAMER_PLUGINSBASE_netbuffer_HEADER "gstnetbuffer.h")
set(GSTREAMER_PLUGINSBASE_pbutils_HEADER "pbutils.h")
set(GSTREAMER_PLUGINSBASE_riff_HEADER "riff-ids.h")
set(GSTREAMER_PLUGINSBASE_rtp_HEADER "gstrtpbuffer.h")
set(GSTREAMER_PLUGINSBASE_rtsp_HEADER "gstrtspdefs.h")
set(GSTREAMER_PLUGINSBASE_sdp_HEADER "gstsdp.h")
set(GSTREAMER_PLUGINSBASE_tag_HEADER "tag.h")



# If the user specified required components e.g. via
# FIND_PACKAGE(GstreamerPluginsBase REQUIRED audio video)
# find (just) those libraries. Otherwise find all libraries.
if(GStreamerPluginsBase_FIND_COMPONENTS)
    set(GSTREAMER_PLUGINSBASE_FIND_COMPONENTS ${GStreamerPluginsBase_FIND_COMPONENTS})
else(GStreamerPluginsBase_FIND_COMPONENTS)
    set(GSTREAMER_PLUGINSBASE_FIND_COMPONENTS ${GSTREAMER_PLUGINSBASE_ALL_COMPONENTS})
endif(GStreamerPluginsBase_FIND_COMPONENTS)


foreach(component ${GSTREAMER_PLUGINSBASE_FIND_COMPONENTS})
  macro_find_gstreamer_plugin(${component} ${GSTREAMER_PLUGINSBASE_${component}_HEADER})
endforeach(component)


if(GStreamerPluginsBase_FOUND)

  list(REMOVE_DUPLICATES GSTREAMERPLUGINSBASE_LIBRARIES)
  list(REMOVE_DUPLICATES GSTREAMERPLUGINSBASE_INCLUDE_DIRS)

  if(NOT GStreamerPluginsBase_FIND_QUIETLY)

    set(PLUGINS_FOUND "")
    foreach(plugin ${GSTREAMER_PLUGINSBASE_FIND_COMPONENTS})
      string(TOUPPER ${plugin} _upper)
      if( GSTREAMER_PLUGIN_${_upper}_LIBRARY )
        set(PLUGINS_FOUND ${PLUGINS_FOUND} "\n\t" ${plugin})
      endif()
    endforeach(plugin)

    #remove empty lines
    string(REGEX REPLACE "\n[ ]*(\n[ ]*)*" "\n"  PLUGINS_FOUND ${PLUGINS_FOUND})
    message(STATUS "Found GStreamer Plugins: ${PLUGINS_FOUND}")

  endif(NOT GStreamerPluginsBase_FIND_QUIETLY)

else(GStreamerPluginsBase_FOUND)

  set(GSTREAMERPLUGINSBASE_LIBRARIES GSTREAMERPLUGINSBASE_LIBRARIES-NOTFOUND)
  set(GSTREAMERPLUGINSBASE_INCLUDE_DIRS GSTREAMERPLUGINSBASE_INCLUDE_DIRS-NOTFOUND)
  if(GStreamerPluginsBase_FIND_REQUIRED)
    message(SEND_ERROR "Could NOT find GStreamer Plugins-Base")
  endif(GStreamerPluginsBase_FIND_REQUIRED)
endif(GStreamerPluginsBase_FOUND)
