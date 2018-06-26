# Enables C++11 on GNU compilers 

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -std=gnu++0x")
    add_definitions("-std=c++0x -std=gnu++0x")
endif()
