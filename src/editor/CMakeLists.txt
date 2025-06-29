find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)
pkg_search_module(GLEW REQUIRED glew)
find_path(GLM_INCLUDE_DIR glm/glm.hpp)

# Include directories
include_directories(
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR}
    ${LIB_DIR}/imgui
    ${CORE_DIR}
    ${EDITOR_DIR}
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

# Create shaders directory in bin and copy shader files
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/shaders)
configure_file(${EDITOR_DIR}/renderer/shaders/basic.vert ${CMAKE_BINARY_DIR}/bin/shaders/basic.vert COPYONLY)
configure_file(${EDITOR_DIR}/renderer/shaders/basic.frag ${CMAKE_BINARY_DIR}/bin/shaders/basic.frag COPYONLY)
configure_file(${EDITOR_DIR}/renderer/shaders/wireframe.vert ${CMAKE_BINARY_DIR}/bin/shaders/wireframe.vert COPYONLY)
configure_file(${EDITOR_DIR}/renderer/shaders/wireframe.frag ${CMAKE_BINARY_DIR}/bin/shaders/wireframe.frag COPYONLY)

# Create assets directory structure in bin
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/assets/fonts)

# Copy font assets if they exist
file(GLOB FONT_FILES "${CMAKE_SOURCE_DIR}/assets/fonts/*")
foreach(FONT_FILE ${FONT_FILES})
    get_filename_component(FONT_NAME ${FONT_FILE} NAME)
    configure_file(${FONT_FILE} ${CMAKE_BINARY_DIR}/bin/assets/fonts/${FONT_NAME} COPYONLY)
endforeach()

add_executable(${PROJECT_NAME}Editor
    ${EDITOR_DIR}/main.cpp
    ${EDITOR_DIR}/Application.cpp
    ${EDITOR_DIR}/window/Window.cpp
    ${EDITOR_DIR}/workbench/Workbench.cpp
    ${EDITOR_DIR}/workbench/Viewport.cpp
    ${EDITOR_DIR}/workbench/Toolkit.cpp
    ${EDITOR_DIR}/workbench/Settings.cpp
    ${EDITOR_DIR}/scene/Scene.cpp
    ${EDITOR_DIR}/renderer/Mesh.cpp
    ${EDITOR_DIR}/renderer/Tessellator.cpp
    ${EDITOR_DIR}/renderer/Renderer.cpp
    ${EDITOR_DIR}/renderer/GridRenderer.cpp
    ${EDITOR_DIR}/renderer/VertexArray.cpp
    ${EDITOR_DIR}/renderer/Camera.cpp
    ${EDITOR_DIR}/renderer/Shader.cpp
    ${EDITOR_DIR}/logger/Logger.cpp
    ${EDITOR_DIR}/ui/FontManager.cpp
    ${EDITOR_DIR}/ui/IconUtils.cpp
)

target_link_libraries(${PROJECT_NAME}Editor
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GLFW_LIBRARIES}
    imgui
    imgui_backend
    MizanCore
)