##############################################################################
# @file  FindOpenCV.cmake
# @brief Find OpenCV Library (http://sourceforge.net/projects/opencvlibrary/)
#
# @par 1. Setup
#
# The following variables are optionally searched for defaults
#  OpenCV_ROOT:            Base directory of OpenCv tree to use.
#
# @par 2. Variable
#
# The following are set after configuration is done:
# - OpenCV_FOUND
# - OpenCV_LIBRARIES
# - OpenCV_INCLUDE_DIR
# - OpenCV_LIBRARY_DIRS
# - OpenCV_VERSION (OpenCV_VERSION_MAJOR, OpenCV_VERSION_MINOR, OpenCV_VERSION_REVISION)
#
#
# The following variables are used to maintain compatibility with other
# Find<Pkg>.cmake modules, including the FindOpenCV.cmake module of
# Jan Woetzel (2006/09, www.mip.informatik.uni-kiel.de/~jw):
# - OpenCV_INCLUDE_DIRS
# - OpenCV_LIBS
# - OpenCV_LINK_DIRECTORIES
#
# @par 3. Version
#
# 2012/10/22 Andreas Schuh, Find OpenCV 2 also if OpenCVConfig.cmake missing.
# 2012/02/28 Andreas Schuh, Reimplemented module to work also for OpenCV 1.x.
# 2010/04/07 Benoit Rat, Correct a bug when OpenCVConfig.cmake is not found.
# 2010/03/24 Benoit Rat, Add compatibility for when OpenCVConfig.cmake is not found.
# 2010/03/22 Benoit Rat, Creation of the script.
#
#
# tested with:
# - OpenCV 2.1:  MinGW, MSVC2008
# - OpenCV 2.0:  MinGW, MSVC2008, GCC4
#
# @par 4. Licence
#
# LGPL 2.1 : GNU Lesser General Public License Usage
# Alternatively, this file may be used under the terms of the GNU Lesser
#
# General Public License version 2.1 as published by the Free Software
# Foundation and appearing in the file LICENSE.LGPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU Lesser General Public License version 2.1 requirements
# will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
#
# @ingroup CMakeFindModules
##############################################################################

# ----------------------------------------------------------------------------
# initialize search
set(OpenCV_FOUND FALSE)

# 1. set OpenCV_ROOT from environment variables
if(NOT OpenCV_ROOT)
  if(DEFINED ENV{OpenCV_ROOT})
    set(OpenCV_ROOT "$ENV{OpenCV_ROOT}" CACHE PATH "Installation prefix of OpenCV Library." FORCE)
  elseif(DEFINED ENV{OpenCV_ROOT})
    set(OpenCV_ROOT "$ENV{OpenCV_ROOT}" CACHE PATH "Installation prefix of OpenCV Library." FORCE)
  endif()
endif()


# 2. otherwise, try to derive it from include path
if(NOT OpenCV_ROOT)
  # a) look for include path which might be easily found using system default
  #    paths such as C_INCLUDE_PATH or CXX_INCLUDE_PATH
  find_path(
    OpenCV_INCLUDE_DIR "cv.h"
    PATH_SUFFIXES "include" "include/opencv"
    DOC "Directory of cv.h header file."
  )
  
  mark_as_advanced(OpenCV_INCLUDE_DIR)
  # b) derive OpenCV_ROOT from include path
  if(OpenCV_INCLUDE_DIR)
    # Mac OS Framework
    string(REGEX REPLACE "/Headers(/.*)?$" "" OpenCV_TMPROOT "${OpenCV_INCLUDE_DIR}")
    # unix
    string(REGEX REPLACE "/include(/.*)$" "" OpenCV_TMPROOT "${OpenCV_INCLUDE_DIR}")
    # cache it such that users can view/correct it
    set(OpenCV_ROOT "${OpenCV_TMPROOT}" CACHE PATH "Installation prefix of OpenCV Library." FORCE)
  endif()
endif()

set(OpenCV_LIBS)                # found libraries
set(OpenCV_COMPONENTS_REQUIRED) # requested components
set(OpenCV_LIB_COMPONENTS)      # found components
set(OpenCV_VERSION)             # found version

# ----------------------------------------------------------------------------
# find headers and libraries
if(EXISTS "${OpenCV_ROOT}")

  # --------------------------------------------------------------------------
  # OpenCV 2
  if(EXISTS "${OpenCV_ROOT}/share/OpenCV/OpenCVConfig.cmake")

    include("${OpenCV_ROOT}/share/OpenCV/OpenCVConfig.cmake")

    foreach(__CVLIB IN LISTS OpenCV_COMPONENTS)
      if(NOT __CVLIB MATCHES "^opencv_")
        set(__CVLIB "opencv_${__CVLIB}")
      endif()
      list(APPEND OpenCV_COMPONENTS_REQUIRED "${__CVLIB}")
    endforeach()

    # Note that OpenCV 2.0.0 does only call the command include_directories()
    # but does not set OpenCV_INCLUDE_DIRS. This variable was added to
    # OpenCVConfig.cmake since version 2.1.0 of OpenCV.
    get_directory_property(__INCLUDE_DIRS INCLUDE_DIRECTORIES)
    find_path(
      OpenCV_INCLUDE_DIR "cv.h"
      HINTS ${__INCLUDE_DIRS}
      DOC "Directory of cv.h header file."
      NO_DEFAULT_PATH
    )
    mark_as_advanced(OpenCV_INCLUDE_DIR)
    unset(__INCLUDE_DIRS)

  # --------------------------------------------------------------------------
  # OpenCV 1 (or OpenCV 2 with missing OpenCVConfig.cmake file)
  else()

    # will be adjusted on Unix to find the correct library version
    set(OpenCV_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES "${CMAKE_FIND_LIBRARY_SUFFIXES}")

    # find include directory
    find_path(
      OpenCV_INCLUDE_DIR "cv.h"
      PATHS "${OpenCV_ROOT}"
      PATH_SUFFIXES "include" "include/opencv"
      DOC "Directory of cv.h header file."
      NO_DEFAULT_PATH
    )

    mark_as_advanced(OpenCV_INCLUDE_DIR)

    if(EXISTS ${OpenCV_INCLUDE_DIR})
      # should not be done by Find module, but OpenCVConfig.cmake does it
      # as well, unfortunately...
      include_directories(${OpenCV_INCLUDE_DIR})
      # extract version information from header file
      if(EXISTS "${OpenCV_INCLUDE_DIR}/cvver.h")
        file(STRINGS "${OpenCV_INCLUDE_DIR}/cvver.h" OpenCV_VERSIONS_TMP REGEX "^#define CV_[A-Z]+_VERSION[ \t]+[0-9]+$")
      elseif(EXISTS "${OpenCV_INCLUDE_DIR}/../opencv2/core/version.hpp")
        file(STRINGS "${OpenCV_INCLUDE_DIR}/../opencv2/core/version.hpp" OpenCV_VERSIONS_TMP REGEX "^#define CV_[A-Z]+_VERSION[ \t]+[0-9]+$")
      else()
        message(FATAL_ERROR "Missing ${OpenCV_INCLUDE_DIR}/cvver.h or ${OpenCV_INCLUDE_DIR}/../opencv2/core/version.hpp file!")
      endif()

      set(THREE_PART_VERSION_REGEX "^[0-9]+\\.[0-9]+\\.[0-9]+")

      # old (1.x?) style version information
      if(NOT "${OpenCV_VERSIONS_TMP}" STREQUAL "")
        string(REGEX REPLACE ".*#define CV_MAJOR_VERSION[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_MAJOR ${OpenCV_VERSIONS_TMP})
        string(REGEX REPLACE ".*#define CV_MINOR_VERSION[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_MINOR ${OpenCV_VERSIONS_TMP})
        string(REGEX REPLACE ".*#define CV_SUBMINOR_VERSION[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_REVISION ${OpenCV_VERSIONS_TMP})
        set(OpenCV_VERSION "${OpenCV_VERSION_MAJOR}.${OpenCV_VERSION_MINOR}.${OpenCV_VERSION_REVISION}")
      else()
      # new (2.x?) style version information
        file(STRINGS "${OpenCV_INCLUDE_DIR}/../opencv2/core/version.hpp" OpenCV_VERSIONS_TMP REGEX "^#define CV_VERSION_[A-Z]+[ \t]+[0-9]+$")
        string(REGEX REPLACE ".*#define CV_VERSION_EPOCH[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_MAJOR ${OpenCV_VERSIONS_TMP})
        string(REGEX REPLACE ".*#define CV_VERSION_MAJOR[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_MINOR ${OpenCV_VERSIONS_TMP})
        string(REGEX REPLACE ".*#define CV_VERSION_MINOR[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_REVISION ${OpenCV_VERSIONS_TMP})
        set(OpenCV_VERSION "${OpenCV_VERSION_MAJOR}.${OpenCV_VERSION_MINOR}.${OpenCV_VERSION_REVISION}")

        # new (3.x?) style version information
        if(NOT ${OpenCV_VERSION} MATCHES ${THREE_PART_VERSION_REGEX})
          string(REGEX REPLACE ".*#define CV_VERSION_MAJOR[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_MAJOR ${OpenCV_VERSIONS_TMP})
          string(REGEX REPLACE ".*#define CV_VERSION_MINOR[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_MINOR ${OpenCV_VERSIONS_TMP})
          string(REGEX REPLACE ".*#define CV_VERSION_REVISION[ \t]+([0-9]+).*" "\\1" OpenCV_VERSION_REVISION ${OpenCV_VERSIONS_TMP})
          set(OpenCV_VERSION "${OpenCV_VERSION_MAJOR}.${OpenCV_VERSION_MINOR}.${OpenCV_VERSION_REVISION}")
        endif()
      endif()

      if(NOT ${OpenCV_VERSION} MATCHES ${THREE_PART_VERSION_REGEX})
        message(FATAL_ERROR "Could not find reliable version information. found: ${OpenCV_VERSION}")
        set(OPENCV_FOUND FALSE)
      endif()

      if(DEFINED OpenCV_FIND_VERSION)
        if(${OpenCV_VERSION} VERSION_LESS ${OpenCV_FIND_VERSION})
          message(FATAL_ERROR "Could not find required version. found: ${OpenCV_VERSION}")
          set(OPENCV_FOUND FALSE)
        endif()
      endif()

      # file name suffixes
      if(UNIX)
        set(OpenCV_CVLIB_NAME_SUFFIX)
        set(CMAKE_FIND_LIBRARY_SUFFIXES)
        foreach(SUFFIX IN LISTS OpenCV_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES)
          if(NOT SUFFIX MATCHES "\\.${OpenCV_VERSION_MAJOR}\\.${OpenCV_VERSION_MINOR}\\.${OpenCV_VERSION_REVISION}$")
            set(SUFFIX "${SUFFIX}.${OpenCV_VERSION}")
          endif()
          list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES "${SUFFIX}")
        endforeach()
        # for the 1.1pre1 version, the suffix of the libraries is by default .2.0.0 instead of .1.1.0
        # thus consider these library files as well, assuming that the suffix has not been corrected
        if(OpenCV_VERSION VERSION_EQUAL 1.1.0)
          foreach(SUFFIX IN LISTS OpenCV_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES)
            if(NOT SUFFIX MATCHES "\\.2\\.0\\.0$")
              list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES "${SUFFIX}.2.0.0")
            endif()
          endforeach()
        endif()
      else()
        set(OpenCV_CVLIB_NAME_SUFFIX "${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_REVISION}")
      endif()
    endif()

    # library components
    if(OpenCV_VERSION_MAJOR GREATER 1)
      set(OpenCV_LIB_COMPONENTS core
                              imgproc
                              highgui
                              ml
                              video
                              features2d
                              calib3d
                              objdetect
                              contrib
                              legacy
                              flann )
    else()
      set(OpenCV_LIB_COMPONENTS cxcore cv ml highgui cvaux)
    endif()

    if(OpenCV_COMPONENTS)
      foreach(__CVLIB IN LISTS OpenCV_COMPONENTS)
        string(REGEX REPLACE "^opencv_" "" __CVLIB__ "${__CVLIB}")
        list(FIND OpenCV_LIB_COMPONENTS ${__CVLIB__} IDX)
        if(IDX EQUAL -1)
          message(FATAL_ERROR "Unknown OpenCV library component: ${__CVLIB}"
                               " Are you looking for OpenCV 2.0.0 or greater?"
                               " In this case, please set OpenCV_ROOT to the"
                               " directory containing the OpenCVConfig.cmake file.")
        endif()
        list(APPEND OpenCV_COMPONENTS_REQUIRED "${__CVLIB__}")
      endforeach()
    else()
      set(OpenCV_COMPONENTS_REQUIRED ${OpenCV_LIB_COMPONENTS})
    endif()

    # find libraries of components
    set(OpenCV_LIB_COMPONENTS)
    foreach(__CVLIB IN LISTS OpenCV_COMPONENTS_REQUIRED)

      # debug build
      find_library(
        OpenCV_${__CVLIB}_LIBRARY_DEBUG
        NAMES "opencv_${__CVLIB}${OpenCV_CVLIB_NAME_SUFFIX}d" "${__CVLIB}${OpenCV_CVLIB_NAME_SUFFIX}d"
        PATHS "${OpenCV_ROOT}/lib"
        NO_DEFAULT_PATH
      )

      # release build
      if(APPLE AND OpenCV_ROOT MATCHES "/OpenCV\\.framework/*$" AND EXISTS "${OpenCV_ROOT}/OpenCV" AND NOT IS_DIRECTORY "${OpenCV_ROOT}/OpenCV")
        find_file(
          OpenCV_${__CVLIB}_LIBRARY_RELEASE
          NAMES OpenCV
          PATHS "${OpenCV_ROOT}"
          NO_DEFAULT_PATH
        )
      else()
        find_library(
          OpenCV_${__CVLIB}_LIBRARY_RELEASE
          NAMES "opencv_${__CVLIB}${OpenCV_CVLIB_NAME_SUFFIX}" "${__CVLIB}${OpenCV_CVLIB_NAME_SUFFIX}"
          PATHS "${OpenCV_ROOT}/lib"
          NO_DEFAULT_PATH
        )
      endif()

      mark_as_advanced(OpenCV_${__CVLIB}_LIBRARY_DEBUG)
      mark_as_advanced(OpenCV_${__CVLIB}_LIBRARY_RELEASE)

      # both debug/release
      if(OpenCV_${__CVLIB}_LIBRARY_DEBUG AND OpenCV_${__CVLIB}_LIBRARY_RELEASE)
        set(OpenCV_${__CVLIB}_LIBRARY debug ${OpenCV_${__CVLIB}_LIBRARY_DEBUG} optimized ${OpenCV_${__CVLIB}_LIBRARY_RELEASE})
      # only debug
      elseif(OpenCV_${__CVLIB}_LIBRARY_DEBUG)
        set(OpenCV_${__CVLIB}_LIBRARY ${OpenCV_${__CVLIB}_LIBRARY_DEBUG})
      # only release
      elseif(OpenCV_${__CVLIB}_LIBRARY_RELEASE)
        set(OpenCV_${__CVLIB}_LIBRARY ${OpenCV_${__CVLIB}_LIBRARY_RELEASE})
      # not found
      else()
        set(OpenCV_${__CVLIB}_LIBRARY)
      endif()

      # add to list of found libraries
      if(OpenCV_${__CVLIB}_LIBRARY)
        list(APPEND OpenCV_LIB_COMPONENTS ${__CVLIB})
        list(APPEND OpenCV_LIBS "${OpenCV_${__CVLIB}_LIBRARY}")
      endif()

    endforeach()

    # restore library suffixes
    set(CMAKE_FIND_LIBRARY_SUFFIXES "${OpenCV_ORIG_CMAKE_FIND_LIBRARY_SUFFIXES}")

    # compatibility with OpenCV 2
    set(OpenCV_INCLUDE_DIRS "${OpenCV_INCLUDE_DIR}")

  endif()

  # --------------------------------------------------------------------------
  # set OpenCV_INCLUDE_DIRS - required for OpenCV before version 2.1.0
  if(OpenCV_INCLUDE_DIR MATCHES "/opencv$" AND NOT OpenCV_INCLUDE_DIRS)
    get_filename_component(OpenCV_INCLUDE_DIRS "${OpenCV_INCLUDE_DIR}" PATH)
    list(APPEND OpenCV_INCLUDE_DIRS "${OpenCV_INCLUDE_DIR}")
  endif()

  # --------------------------------------------------------------------------
  # handle the QUIETLY and REQUIRED arguments and set *_FOUND to TRUE
  # if all listed variables are found or TRUE
  include(FindPackageHandleStandardArgs)

  set(OpenCV_REQUIRED_COMPONENTS_FOUND TRUE)
  set(OpenCV_COMPONENTS_NOT_FOUND)
  foreach(__CVLIB IN LISTS OpenCV_COMPONENTS_REQUIRED)
    list(FIND OpenCV_LIB_COMPONENTS ${__CVLIB} IDX)
    if(IDX EQUAL -1)
      set(OpenCV_REQUIRED_COMPONENTS_FOUND FALSE)
      list(APPEND OpenCV_COMPONENTS_NOT_FOUND ${__CVLIB})
    endif()
  endforeach()

  if(NOT OpenCV_REQUIRED_COMPONENTS_FOUND)
    if(NOT OpenCV_FIND_QUIET AND OpenCV_FIND_REQUIRED)
      message(FATAL_ERROR "The following required OpenCV components"
                           " were not found: ${OpenCV_COMPONENTS_NOT_FOUND}")
    endif()
  endif()

  find_package_handle_standard_args(
    OpenCV
    REQUIRED_VARS
      OpenCV_INCLUDE_DIR
      OpenCV_LIBS
      OpenCV_REQUIRED_COMPONENTS_FOUND
    VERSION_VAR
      OpenCV_VERSION
  )

  set(OpenCV_FOUND "${OPENCV_FOUND}")

  # --------------------------------------------------------------------------
  # (backward) compatibility
  if(OpenCV_FOUND)
    set(OpenCV_LIBRARIES "${OpenCV_LIBS}")
  endif()

elseif(NOT OpenCV_FIND_QUIET AND OpenCV_FIND_REQUIRED)
  message(FATAL_ERROR "Please specify the OpenCV directory using OpenCV_ROOT (environment) variable.")
endif()
