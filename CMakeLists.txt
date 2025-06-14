cmake_minimum_required(VERSION 3.16)
project(Simple3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)
find_path(GLM_INCLUDE_DIR glm/glm.hpp)

set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR}
    ${LIB_DIR}/imgui
)

# Source files
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Application.cpp
    src/ui/Window.cpp
    src/modeller/Edge.cpp
    src/modeller/Face.cpp
    src/modeller/Vertex.cpp
    src/modeller/HalfEdge.cpp
    src/modeller/Loop.cpp 
    src/modeller/Shell.cpp 
    src/modeller/Solid.cpp
    src/modeller/BREPBuilder.cpp
    src/renderer/Tesselator.cpp
    src/renderer/Renderer.cpp
    src/renderer/VertexArray.cpp
    src/renderer/Camera.cpp
    src/renderer/Shader.cpp
)

add_library(imgui STATIC
    ${LIB_DIR}/imgui/imgui.cpp
    ${LIB_DIR}/imgui/imgui_draw.cpp
    ${LIB_DIR}/imgui/imgui_demo.cpp
    ${LIB_DIR}/imgui/imgui_tables.cpp
    ${LIB_DIR}/imgui/imgui_widgets.cpp
)

target_include_directories(imgui PUBLIC ${LIB_DIR}/imgui)

# Add ImGui backend
add_library(imgui_backend STATIC
    ${LIB_DIR}/imgui/backends/imgui_impl_glfw.cpp
    ${LIB_DIR}/imgui/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_backend PUBLIC
    ${LIB_DIR}/imgui
    ${LIB_DIR}/imgui/backends
)

target_link_libraries(imgui_backend imgui glfw OpenGL::GL)

# Copy shader files to binary directory
configure_file(${CMAKE_SOURCE_DIR}/shaders/basic.vert ${CMAKE_BINARY_DIR}/shaders/basic.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/basic.frag ${CMAKE_BINARY_DIR}/shaders/basic.frag COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/wireframe.vert ${CMAKE_BINARY_DIR}/shaders/wireframe.vert COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/shaders/wireframe.frag ${CMAKE_BINARY_DIR}/shaders/wireframe.frag COPYONLY)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    imgui
    imgui_backend
)
