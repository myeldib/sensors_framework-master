# - Try to find FindGigEAvt
#
# You can define to use shared or static libs with:
#  GIGEAVT_SHARED = 1 (defaults to 0)
#
# Once done, this will define
#  GIGEAVT_FOUND
#  GIGEAVT_INCLUDE_DIRS
#  GIGEAVT_LIBRARIES
#  GIGEAVT_LIBRARY_DIRS
#  GIGEAVT_DEFINITIONS

set(GIGEAVT_FOUND FALSE)

find_path(GIGEAVT_INCLUDE_DIRS "PvApi.h"
            HINTS ${GIGEAVT_DIR} ENV ProgramFiles ENV ProgramW6432 /opt /usr/local /usr
            PATH_SUFFIXES "Allied Vision Technologies/GigESDK/inc-pc" "AVT GigE SDK/inc-pc" "GigESDK/inc-pc" "inc-pc"
            DOC "The path to PvAPI header" NO_DEFAULT_PATH)

if(GIGEAVT_INCLUDE_DIRS)

    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(PVAPI_SDK_SUBDIR x64)
      set(GIGEAVT_DEFINITIONS " -D _x64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES arm)
      set(PVAPI_SDK_SUBDIR arm)
    else(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(PVAPI_SDK_SUBDIR x86)
      set(GIGEAVT_DEFINITIONS " -D _x86")
    endif(CMAKE_SIZEOF_VOID_P EQUAL 8)

    if(UNIX)
       set(GIGEAVT_DEFINITIONS "${GIGEAVT_DEFINITIONS} -D _LINUX")
    endif(UNIX)

    get_filename_component(_PVAPI_BASE "${GIGEAVT_INCLUDE_DIRS}/../" ABSOLUTE)

    if(GIGEAVT_SHARED)
        #get_filename_component(_PVAPI_LIBRARY_DIR "${_PVAPI_BASE}/bin-pc" ABSOLUTE)
        #get_filename_component(_PVAPI_LIBRARY "${_PVAPI_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}PvAPI${CMAKE_SHARED_LIBRARY_SUFFIX}" NAME)

        #if(EXISTS "${_PVAPI_LIBRARY_DIR}")
        #    set(GIGEAVT_LIBRARIES ${_PVAPI_LIBRARY} CACHE FILEPATH "The PvAPI shared library")
        #    set(GIGEAVT_LIBRARY_DIRS ${_PVAPI_LIBRARY_DIR} CACHE PATH "The PvAPI library dir")
        #    set(GIGEAVT_FOUND TRUE)
        #endif()

        get_filename_component(_PVAPI_LIBRARY "${_PVAPI_BASE}/bin-pc" ABSOLUTE)

        if(PVAPI_SDK_SUBDIR)
            set(_PVAPI_LIBRARY "${_PVAPI_LIBRARY}/${PVAPI_SDK_SUBDIR}")
        endif()

        set(GIGEAVT_LIBRARIES "${_PVAPI_LIBRARY}/${CMAKE_SHARED_LIBRARY_PREFIX}PvAPI${CMAKE_SHARED_LIBRARY_SUFFIX}" CACHE PATH "The PvAPI shared library")

        if(EXISTS "${GIGEAVT_LIBRARIES}")
            set(GIGEAVT_LIBRARY_DIRS ${_PVAPI_LIBRARY} CACHE PATH "The PvAPI library dir")
            set(GIGEAVT_FOUND TRUE)
        endif()

    else(GIGEAVT_SHARED)

        get_filename_component(_PVAPI_LIBRARY "${_PVAPI_BASE}/lib-pc" ABSOLUTE)

        if(PVAPI_SDK_SUBDIR)
            set(_PVAPI_LIBRARY "${_PVAPI_LIBRARY}/${PVAPI_SDK_SUBDIR}")
        endif()
        if(NOT WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
            execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
             
            if(NOT "${GCC_VERSION}" STREQUAL "")
            	string(REGEX MATCHALL "[0-9]+" GCC_VERSION_COMPONENTS ${GCC_VERSION})
            	list(GET GCC_VERSION_COMPONENTS 0 GCC_MAJOR)
            	list(GET GCC_VERSION_COMPONENTS 1 GCC_MINOR)

	   	 # pvapi only supports gcc up to 4.5, but it actually works in any higher version
            	if("${GCC_MAJOR}.${GCC_MINOR}" VERSION_GREATER "4.5")
            	  set(_PVAPI_LIBRARY "${_PVAPI_LIBRARY}/4.5")
            	else("${GCC_MAJOR}.${GCC_MINOR}" VERSION_GREATER "4.5")
              	set(_PVAPI_LIBRARY "${_PVAPI_LIBRARY}/${GCC_MAJOR}.${GCC_MINOR}")
            	endif("${GCC_MAJOR}.${GCC_MINOR}" VERSION_GREATER "4.5")
            else(NOT "${GCC_VERSION}" STREQUAL "")
                # gcc version not found. Just default to 4.5
            	set(_PVAPI_LIBRARY "${_PVAPI_LIBRARY}/4.5")
            endif(NOT "${GCC_VERSION}" STREQUAL "")
        endif()

        set(GIGEAVT_LIBRARIES "${_PVAPI_LIBRARY}/${CMAKE_STATIC_LIBRARY_PREFIX}PvAPI${CMAKE_STATIC_LIBRARY_SUFFIX}" CACHE FILEPATH "The PvAPI static library")

        if(EXISTS "${GIGEAVT_LIBRARIES}")
            set(GIGEAVT_LIBRARY_DIRS ${_PVAPI_LIBRARY} CACHE PATH "The PvAPI library dir")
            set(GIGEAVT_FOUND TRUE)
        endif()

    endif(GIGEAVT_SHARED)

endif(GIGEAVT_INCLUDE_DIRS)

mark_as_advanced(GIGEAVT_FOUND GIGEAVT_INCLUDE_DIRS GIGEAVT_LIBRARIES GIGEAVT_LIBRARY_DIRS GIGEAVT_DEFINITIONS)

