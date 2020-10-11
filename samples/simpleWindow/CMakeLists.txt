# build process for the simpleWindow sample
add_executable(simpleWindow main.cpp)
target_link_libraries(simpleWindow LINK_PUBLIC ${PROJECT_NAME})
target_include_directories(simpleWindow PUBLIC ${CMAKE_SOURCE_DIR}/inc/ ${CMAKE_SOURCE_DIR}/dep/)
add_compile_definitions(DIRECTINPUT_VERSION=0x0800 NOMINMAX)
physx_dependency(simpleWindow)