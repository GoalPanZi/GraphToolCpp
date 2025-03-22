cmake_minimum_required(VERSION 3.10)
project(GraphTool)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MAKE_PROGRAM "Y:/DEV/mingw64/bin/mingw32-make.exe")
set(CMAKE_C_COMPILER "Y:/DEV/mingw64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "Y:/DEV/mingw64/bin/g++.exe")

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

link_directories(
    ${CMAKE_SOURCE_DIR}/lib/glfw
    ${CMAKE_SOURCE_DIR}/lib/glew
)

set(IMGUI_SOURCES
    ${CMAKE_SOURCE_DIR}/src/imgui.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui_tables.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui_impl_opengl3.cpp
    ${CMAKE_SOURCE_DIR}/src/imgui_impl_glfw.cpp
)

set(UTILS_SOURCES
    ${CMAKE_SOURCE_DIR}/src/object_model.cpp
    ${CMAKE_SOURCE_DIR}/src/renderer.cpp
    ${CMAKE_SOURCE_DIR}/src/graph_app.cpp
)

set(UI_SOURCES
    ${CMAKE_SOURCE_DIR}/src/UI/object_list_panel.cpp
)

file(COPY ${CMAKE_SOURCE_DIR}/src/shaders DESTINATION ${CMAKE_BINARY_DIR})

add_executable(GraphTool src/main.cpp ${UTILS_SOURCES} ${UI_SOURCES} ${IMGUI_SOURCES})

target_link_libraries(GraphTool glew32 glfw3 opengl32)
