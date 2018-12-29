project("SvrBase")

set(CMAKE_CXX_FLAGS "-std=gnu++0x -fPIC")

macro(add_src_subdirs source_files)
	foreach(_subdir ${ARGN})
		set(_sub_files "")
		aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/${_subdir}/src" _sub_files)
		set(source_files ${_sub_files} ${${source_files}})
		
		include_directories(${CMAKE_CURRENT_SOURCE_DIR}/${_subdir}/include) 
	endforeach(_subdir)
endmacro()

add_src_subdirs(source_files "." "base" "cfg" "libuv")
MESSAGE(STATUS "Src :" ${source_files})

include_directories(../third/libuv/linux/include)
include_directories(../third/rapidjson)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

add_library(${LIB_BPLUSTREE_NAME}_static STATIC ${source_files})


