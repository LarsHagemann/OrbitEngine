set(ORBIT_SOURCE
	base_camera.cpp
	batch_component.cpp
	component.cpp
	engine.cpp
	engine_callback.cpp
	engine_initialization.cpp
	exception.cpp
	helper.cpp
	keyboard_component.cpp
	light.cpp
	material.cpp
	mesh.cpp
	mouse_component.cpp
	object.cpp
	resource_manager.cpp
	scene.cpp
	third_person_camera.cpp
	time.cpp
	transform.cpp
)

add_library(Orbit ${ORBIT_SOURCE})
target_include_directories(Orbit PUBLIC ../inc/ ../dep/)
add_compile_definitions(DIRECTINPUT_VERSION=0x0800 NOMINMAX)
add_definitions(-DUNICODE)