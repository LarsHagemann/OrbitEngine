source_group(zlib
	FILES
	
	${ZLIB_DIR}/adler32.c
	${ZLIB_DIR}/compress.c
	${ZLIB_DIR}/crc32.c
	${ZLIB_DIR}/deflate.c
	${ZLIB_DIR}/gzclose.c
    ${ZLIB_DIR}/gzlib.c
    ${ZLIB_DIR}/gzread.c
    ${ZLIB_DIR}/gzwrite.c
	${ZLIB_DIR}/infback.c
	${ZLIB_DIR}/inffast.c
	${ZLIB_DIR}/inflate.c
	${ZLIB_DIR}/inftrees.c
	${ZLIB_DIR}/trees.c
	${ZLIB_DIR}/uncompr.c
	${ZLIB_DIR}/zutil.c
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

    ${ZLIB_DIR}/adler32.c
	${ZLIB_DIR}/compress.c
	${ZLIB_DIR}/crc32.c
	${ZLIB_DIR}/deflate.c
	${ZLIB_DIR}/gzclose.c
    ${ZLIB_DIR}/gzlib.c
    ${ZLIB_DIR}/gzread.c
    ${ZLIB_DIR}/gzwrite.c
	${ZLIB_DIR}/infback.c
	${ZLIB_DIR}/inffast.c
	${ZLIB_DIR}/inflate.c
	${ZLIB_DIR}/inftrees.c
	${ZLIB_DIR}/trees.c
	${ZLIB_DIR}/uncompr.c
	${ZLIB_DIR}/zutil.c

    src/main.cpp 
)

target_include_directories(orbtool PUBLIC 
    ${CMAKE_SOURCE_DIR}/converter/inc/ 
    ${CMAKE_SOURCE_DIR}/dep/
	${ZLIB_DIR}/
    ../inc/
)
if (NOT "${EIGEN_ROOT_PATH}" STREQUAL "")
    target_include_directories(orbtool PUBLIC ${EIGEN_ROOT_PATH})
endif()
add_compile_definitions(NOMINMAX _ENABLE_EXTENDED_ALIGNED_STORAGE)