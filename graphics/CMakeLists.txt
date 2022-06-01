file(GLOB_RECURSE sources *.h *.cpp)
set(target_name graphics)
add_library(${target_name} ${sources})
set_target_properties(${target_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY
	${PROJECT_SOURCE_DIR}/bin/${target_name})
include_directories(src)
include_directories(${PROJECT_SOURCE_DIR}/lib/aftermath/include)