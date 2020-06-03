cmake_minimum_required(VERSION 3.1)
project(imgui)

set(OUR_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(TARGET              imgui)
set(SRC_DIR             ${OUR_DIR})

set(PUBLIC_HDRS
    ${SRC_DIR}/imconfig.h
    ${SRC_DIR}/imgui.h
    ${SRC_DIR}/misc/stl/imgui_stl.h
)

set(PRIVATE_HDRS
    ${SRC_DIR}/imgui_internal.h
    ${SRC_DIR}/imstb_rectpack.h
    ${SRC_DIR}/imstb_textedit.h
    ${SRC_DIR}/imstb_truetype.h
)

set(SRCS
    ${SRC_DIR}/imgui.cpp
    ${SRC_DIR}/imgui_demo.cpp
    ${SRC_DIR}/imgui_draw.cpp
    ${SRC_DIR}/imgui_widgets.cpp
    ${SRC_DIR}/misc/stl/imgui_stl.cpp
)

if (WIN32)
    list(APPEND PRIVATE_HDRS ${SRC_DIR}/examples/imgui_impl_win32.h)
    list(APPEND SRCS ${SRC_DIR}/examples/imgui_impl_win32.cpp)
endif()


add_library(${TARGET} STATIC ${PRIVATE_HDRS} ${PUBLIC_HDRS} ${SRCS})

# if (WIN32)
#     target_include_directories(${TARGET} 
#     PUBLIC
#         ${NamaEngine_ROOT_DIR}/ThirdParty/imgui/examples
#     )
#     target_link_libraries(${TARGET} 
#     PUBLIC
#         sdl2
#     )
# endif()

target_include_directories(${TARGET} 
PUBLIC
    ${NamaEngine_ROOT_DIR}/ThirdParty/imgui/examples
)

target_include_directories(${TARGET} PUBLIC ${SRC_DIR})
set_target_properties(${TARGET} PROPERTIES FOLDER External)
