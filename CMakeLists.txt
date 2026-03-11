cmake_minimum_required(VERSION 3.16)
project(voxel-engine)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/dist)

# ─── Packages ─────────────────────────────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(glfw3  REQUIRED)

# ─── GLAD ─────────────────────────────────────────────────────────────────────
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include/)

# ─── ImGui ────────────────────────────────────────────────────────────────────
add_library(imgui STATIC
    include/imgui/imgui.cpp
    include/imgui/imgui_draw.cpp
    include/imgui/imgui_tables.cpp
    include/imgui/imgui_widgets.cpp
    include/imgui/imgui_demo.cpp
    include/imgui/imgui_impl_glfw.cpp
    include/imgui/imgui_impl_opengl3.cpp
    include/imgui/imgui_stdlib.cpp
)
target_include_directories(imgui PUBLIC include/ include/imgui/)
target_link_libraries(imgui PUBLIC glfw)

# ─── Main Sources ─────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/stb.cpp
    src/Core/Engine.cpp
    src/Core/Input.cpp
    src/Render/Shader.cpp
    src/Render/Mesh.cpp
    src/Render/Texture.cpp
    src/Render/Camera.cpp
    src/World/Chunk.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include/
    src/
    src/Core/
    src/Render/
    src/World/
)

# ─── Link ─────────────────────────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    glad
    imgui
)

# ─── Copy assets to dist ──────────────────────────────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_SOURCE_DIR}/dist/assets
)

# ─── Auto copy shaders when changed ──────────────────────────────────────────
file(GLOB_RECURSE SHADER_FILES "src/shaders/*")
foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    configure_file(${SHADER} ${CMAKE_SOURCE_DIR}/dist/shaders/${SHADER_NAME} COPYONLY)
endforeach()
