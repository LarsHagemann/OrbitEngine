source_group(zlib
	FILES
	
	${ZLIB_ROOT_PATH}/adler32.c
	${ZLIB_ROOT_PATH}/compress.c
	${ZLIB_ROOT_PATH}/crc32.c
	${ZLIB_ROOT_PATH}/deflate.c
	${ZLIB_ROOT_PATH}/gzclose.c
    ${ZLIB_ROOT_PATH}/gzlib.c
    ${ZLIB_ROOT_PATH}/gzread.c
    ${ZLIB_ROOT_PATH}/gzwrite.c
	${ZLIB_ROOT_PATH}/infback.c
	${ZLIB_ROOT_PATH}/inffast.c
	${ZLIB_ROOT_PATH}/inflate.c
	${ZLIB_ROOT_PATH}/inftrees.c
	${ZLIB_ROOT_PATH}/trees.c
	${ZLIB_ROOT_PATH}/uncompr.c
	${ZLIB_ROOT_PATH}/zutil.c
)

source_group(
    converter
    FILES
    main.cpp
)

source_group(
    orb
    FILES
    orb/OrbFile.cpp
    orb/OrbIntermediate.cpp
)

source_group(
    wavefront
    FILES
    wavefront/MaterialReader.cpp
    wavefront/ObjectReader.cpp
)

source_group(
    fbx
    FILES
    fbx/FbxReader.cpp
    fbx/FbxTree.cpp
)

source_group(
    alembic
    FILES
    alembic/DaeReader.cpp
)

source_group(
    raw
    FILES
    raw/RawReader.cpp
)

source_group(
    misc
    FILES
    ArgumentParser.cpp
    Reader.cpp
    Helper.cpp
    ../../src/implementation/misc/Logger.cpp
    ../../src/implementation/Common.cpp
)

set(
    ORBTOOL_SRC
    
    main.cpp
    ArgumentParser.cpp
    Reader.cpp
    Helper.cpp
    ../../src/implementation/misc/Logger.cpp
    ../../src/implementation/Common.cpp

    orb/OrbFile.cpp
    orb/OrbIntermediate.cpp

    wavefront/MaterialReader.cpp
    wavefront/ObjectReader.cpp

    fbx/FbxReader.cpp
    fbx/FbxTree.cpp

    alembic/DaeReader.cpp

    raw/RawReader.cpp

    ${ZLIB_ROOT_PATH}/adler32.c
	${ZLIB_ROOT_PATH}/compress.c
	${ZLIB_ROOT_PATH}/crc32.c
	${ZLIB_ROOT_PATH}/deflate.c
	${ZLIB_ROOT_PATH}/gzclose.c
    ${ZLIB_ROOT_PATH}/gzlib.c
    ${ZLIB_ROOT_PATH}/gzread.c
    ${ZLIB_ROOT_PATH}/gzwrite.c
	${ZLIB_ROOT_PATH}/infback.c
	${ZLIB_ROOT_PATH}/inffast.c
	${ZLIB_ROOT_PATH}/inflate.c
	${ZLIB_ROOT_PATH}/inftrees.c
	${ZLIB_ROOT_PATH}/trees.c
	${ZLIB_ROOT_PATH}/uncompr.c
	${ZLIB_ROOT_PATH}/zutil.c
)

add_executable(orbtool ${ORBTOOL_SRC})
target_include_directories(orbtool 
	PRIVATE 
		PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/converter/inc/>
		PUBLIC $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}/converter/inc/> 
        PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/inc/>
		PUBLIC $<INSTALL_INTERFACE:${CMAKE_SOURCE_DIR}/inc/> 
		PUBLIC $<BUILD_INTERFACE:${ZLIB_ROOT_PATH}>
		PUBLIC $<INSTALL_INTERFACE:${ZLIB_ROOT_PATH}>
)
if (NOT "${EIGEN_ROOT_PATH}" STREQUAL "")
	target_include_directories(orbtool PUBLIC ${EIGEN_ROOT_PATH})
else()
	message(WARNING "EIGEN_ROOT_PATH not set. Compilation will fail")
endif()

target_compile_definitions(orbtool PUBLIC ORBIT_RENDER_ENGINE="${ORBIT_RENDER_ENGINE}" ORBTOOL_CONV)

if ("${ORBIT_RENDER_ENGINE}" STREQUAL "ORBIT_DIRECTX_11" OR "${ORBIT_RENDER_ENGINE}" STREQUAL "ORBIT_DIRECTX_12")
	target_link_libraries(orbtool PRIVATE "d3dcompiler.lib")
endif()
