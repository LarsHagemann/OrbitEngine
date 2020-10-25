source_group(zlib
	FILES
	
	../src/zlib/adler32.c
	../src/zlib/compress.c
	../src/zlib/crc32.c
	../src/zlib/deflate.c
	../src/zlib/gzclose.c
    ../src/zlib/gzlib.c
    ../src/zlib/gzread.c
    ../src/zlib/gzwrite.c
	../src/zlib/infback.c
	../src/zlib/inffast.c
	../src/zlib/inflate.c
	../src/zlib/inftrees.c
	../src/zlib/trees.c
	../src/zlib/uncompr.c
	../src/zlib/zutil.c
)

source_group(fbx
	FILES

	src/fbx/fbx_convert.cpp
	src/fbx/fbx_read.cpp
)

source_group(orb
	FILES
	
	src/orb/orb.cpp
)

add_executable(orbtool 

	src/fbx/fbx_convert.cpp
	src/fbx/fbx_read.cpp

	src/orb/orb.cpp

    ../src/zlib/adler32.c
	../src/zlib/compress.c
	../src/zlib/crc32.c
	../src/zlib/deflate.c
	../src/zlib/gzclose.c
    ../src/zlib/gzlib.c
    ../src/zlib/gzread.c
    ../src/zlib/gzwrite.c
	../src/zlib/infback.c
	../src/zlib/inffast.c
	../src/zlib/inflate.c
	../src/zlib/inftrees.c
	../src/zlib/trees.c
	../src/zlib/uncompr.c
	../src/zlib/zutil.c

    src/main.cpp 
)

target_include_directories(orbtool PUBLIC 
    ${CMAKE_SOURCE_DIR}/converter/inc/ 
    ${CMAKE_SOURCE_DIR}/dep/
    ../inc/
)
if (NOT "${EIGEN_ROOT_PATH}" STREQUAL "")
    target_include_directories(orbtool PUBLIC ${EIGEN_ROOT_PATH})
endif()
add_compile_definitions(NOMINMAX)