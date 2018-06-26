#
# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIRS
# GLEW_LIBRARIES
# 

if(WIN32)
        find_path( GLEW_INCLUDE_DIRS GL/glew.h
		$ENV{PROGRAMFILES}/GLEW/include
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/include
		DOC "The directory where GL/glew.h resides")
        find_library( GLEW_LIBRARIES
		NAMES glew GLEW glew32 glew32s
		PATHS
		$ENV{PROGRAMFILES}/GLEW/lib
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/bin
		${PROJECT_SOURCE_DIR}/src/nvgl/glew/lib
		DOC "The GLEW library")
else(WIN32)
        find_path( GLEW_INCLUDE_DIRS GL/glew.h
		/usr/include
		/usr/local/include
		/sw/include
		/opt/local/include
		DOC "The directory where GL/glew.h resides")
        find_library( GLEW_LIBRARIES
		NAMES GLEW glew
		PATHS
                /usr/lib/x86_64-linux-gnu
                /usr/lib/i386-linux-gnu
		/usr/lib64
		/usr/lib
		/usr/local/lib64
		/usr/local/lib
		/sw/lib
		/opt/local/lib
		DOC "The GLEW library")
endif(WIN32)

if(GLEW_INCLUDE_DIRS)
        set( GLEW_FOUND 1 CACHE STRING "Set to 1 if GLEW is found, 0 otherwise")
else(GLEW_INCLUDE_DIRS)
        set( GLEW_FOUND 0 CACHE STRING "Set to 1 if GLEW is found, 0 otherwise")
endif(GLEW_INCLUDE_DIRS)

mark_as_advanced(GLEW_FOUND GLEW_INCLUDE_DIRS GLEW_LIBRARIES)
