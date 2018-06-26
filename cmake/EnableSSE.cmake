# Enables SSE1-4.2 on GCC and MSVC

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE /arch:SSE2")
endif(MSVC)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2")
    add_definitions("-msse -msse2 -msse3 -mssse3 -msse4.1 -msse4.2")
endif()

