# - Try to find Qt5
# Once done this will define
#
#  Qt5_FOUND - system has Qt5
#  Qt5_INCLUDE_DIRS - the Qt5 include directories
#  Qt5_LIBRARIES - the libraries needed to use Qt5
#  Qt5_DEFINITIONS - Compiler switches required for using Qt5


set(QT5_ALL_COMPONENTS
Core                    
Gui                     
Widgets                 
OpenGL                  
OpenGLExtensions        
Quick                   
Test                    
Network                 
PrintSupport            
QuickTest               
DBus                    
Qml                     
Sql                     
Xml                     
)

if (CMAKE_VERSION VERSION_LESS 2.8.9)
    message(FATAL_ERROR "Qt5 requires at least CMake version 2.8.9")
endif()

if (NOT Qt5_FIND_COMPONENTS)
    set(Qt5_FIND_COMPONENTS ${QT5_ALL_COMPONENTS})
endif()

set(_Qt5_FIND_PARTS_REQUIRED)
if (Qt5_FIND_REQUIRED)
    set(_Qt5_FIND_PARTS_REQUIRED REQUIRED)
endif()
set(_Qt5_FIND_PARTS_QUIET)
if (Qt5_FIND_QUIETLY)
    set(_Qt5_FIND_PARTS_QUIET QUIET)
endif()

set(_Qt5_NOTFOUND_MESSAGE)

foreach(module ${Qt5_FIND_COMPONENTS})
    find_package(Qt5${module}
        ${_Qt5_FIND_PARTS_QUIET}
        ${_Qt5_FIND_PARTS_REQUIRED}
    )
    if (NOT Qt5${module}_FOUND)
        if (Qt5_FIND_REQUIRED_${module})
            set(_Qt5_NOTFOUND_MESSAGE "${_Qt5_NOTFOUND_MESSAGE}Failed to find Qt5 component \"${module}\" config file at \"${_qt5_install_prefix}/Qt5${module}/Qt5${module}Config.cmake\"\n")
        elseif(NOT Qt5_FIND_QUIETLY)
            message(WARNING "Failed to find Qt5 component \"${module}\" config file at \"${_qt5_install_prefix}/Qt5${module}/Qt5${module}Config.cmake\"")
        endif()
    else (NOT Qt5${module}_FOUND)
        set(Qt5_DEFINITIONS ${Qt5_DEFINITIONS} ${Qt5${module}_DEFINITIONS})
        set(Qt5_INCLUDE_DIRS ${Qt5_INCLUDE_DIRS} ${Qt5${module}_INCLUDE_DIRS})
        set(Qt5_LIBRARIES ${Qt5_LIBRARIES} ${Qt5${module}_LIBRARIES})
    endif (NOT Qt5${module}_FOUND)
endforeach()

if (_Qt5_NOTFOUND_MESSAGE)
    set(Qt5_NOT_FOUND_MESSAGE "${_Qt5_NOTFOUND_MESSAGE}")
    set(Qt5_FOUND False)
endif()


