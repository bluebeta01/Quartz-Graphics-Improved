file(GLOB_RECURSE sources *.h *.cpp)
set(target_name editor)
add_executable(${target_name} ${sources})
set_target_properties(${target_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
	${PROJECT_SOURCE_DIR}/bin/${target_name})
include_directories(src)
include_directories(${PROJECT_SOURCE_DIR}/engine/src)
include_directories(${PROJECT_SOURCE_DIR}/engine/src/include)
include_directories(${PROJECT_SOURCE_DIR}/graphics/include)
include_directories(${PROJECT_SOURCE_DIR}/graphics/src)
include_directories(${PROJECT_SOURCE_DIR}/lib/json)
include_directories(${PROJECT_SOURCE_DIR}/lib/lodepng)
include_directories(${PROJECT_SOURCE_DIR}/lib/spdlog)
include_directories(${PROJECT_SOURCE_DIR}/lib/glm)
include_directories(${PROJECT_SOURCE_DIR}/lib/entt/include)

target_link_libraries(${target_name}
	engine
	lodepng
	graphics
	d3d12.lib
	d3dcompiler.lib
	dxgi.lib
	dxguid.lib
)

target_precompile_headers(${target_name} PRIVATE src/pch.h)

set_target_properties(${target_name} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/build)

add_custom_command(TARGET ${target_name} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target_name}> ${PROJECT_SOURCE_DIR}/build)