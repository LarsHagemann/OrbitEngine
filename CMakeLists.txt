cmake_minimum_required(VERSION "3.12.4")
project("Orbit")

macro(set_option var default type docstring)
    if(NOT DEFINED ${var})
        set(${var} ${default})
    endif()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()

set_option(BUILD_SAMPLES TRUE BOOL "Uncheck this value if you don't want to build the samples.")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)
if (BUILD_SAMPLES)
	add_subdirectory(samples)
endif()