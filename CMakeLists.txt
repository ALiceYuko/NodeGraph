cmake_minimum_required (VERSION 3.6)

project(NodeEdit)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(TARGET NodeEdit)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/thirdparty/CMakeModules)

set(HeaderDIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SrcDIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

include_directories(${HeaderDIR})
include_directories(thirdparty/BlueprintUtilities/Include)
include_directories(thirdparty/gl3w/Include)
include_directories(thirdparty/imgui)
include_directories(thirdparty/imgui_node_editor/Include)
include_directories(thirdparty/rapidjson/include)
include_directories(thirdparty/robin-map/include)

add_definitions(
    -DNE_USE_THIRD_LIB_SELF
)
# file(GLOB_RECURSE BluePrintsHeader ${HeaderDIR}/Blueprints/*.*)

# 接口
set(API 
	${HeaderDIR}/nodeedit.h
	${HeaderDIR}/nodesdef.h
	${SrcDIR}/nodeedit.cpp
	${SrcDIR}/nodesdef.cpp
)

set(DATAMGR
    ${HeaderDIR}/nodemgr.h
    ${SrcDIR}/nodemgr.cpp
)

set(BASE
    ${HeaderDIR}/neutil.h
    ${SrcDIR}/neutil.cpp
	${HeaderDIR}/nebase.h
)

set(BLUEPRINTS
    ${HeaderDIR}/Blueprints/Blueprints.h
	${HeaderDIR}/Blueprints/BlueprintWidget.h
    ${SrcDIR}/Blueprints/Blueprints.cpp
)




add_library(${TARGET} STATIC ${API} ${DATAMGR} ${BLUEPRINTS} ${BASE})
set(ASSETS ${CMAKE_CURRENT_SOURCE_DIR}/res)
set_property(TARGET ${TARGET} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${ASSETS}")

if (MSVC)
source_group("API\\" FILES ${API} )
source_group("DataMgr\\" FILES ${DATAMGR} )
source_group("UI\\BluePrints" FILES ${BLUEPRINTS} )
source_group("Base" FILES ${BASE} )

endif()

find_package(blueprint-utilities REQUIRED)

target_link_libraries(${TARGET} PRIVATE blueprint-utilities)

