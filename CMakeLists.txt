cmake_minimum_required(VERSION 3.16)
project(voxel-engine)

# ─── Settings ─────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put output inside build folder (CI-safe)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/dist)

# ─── Packages ─────────────────────────────────────────────
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

# ─── GLAD ────────────────────────────────────────────────
add_library(glad STATIC src/glad.c)
target_include_directories(glad PUBLIC include/)

# ─── ImGui ───────────────────────────────────────────────
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

target_include_directories(imgui PUBLIC
    include/
    include/imgui/
)

target_link_libraries(imgui PUBLIC glfw)

# ─── Main Executable ─────────────────────────────────────
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/stb.cpp
    src/Core/Engine.cpp
    src/Core/Input.cpp
    src/Render/Shader.cpp
    src/Render/Mesh.cpp
    src/Render/Texture.cpp
    src/Render/Camera.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    include/
    src/
    src/Core/
    src/Render/
)

# ─── Link ────────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    glad
    imgui
)

# Linux sometimes needs this explicitly
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} dl)
endif()

# ─── Assets Copy ─────────────────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

# ─── Shader Auto Copy ────────────────────────────────────
file(GLOB_RECURSE SHADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/src/shaders/*"
)

file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders)

foreach(SHADER ${SHADER_FILES})
    get_filename_component(SHADER_NAME ${SHADER} NAME)

    add_custom_command(
        OUTPUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${SHADER_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy
            ${SHADER}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${SHADER_NAME}
        DEPENDS ${SHADER}
    )

    list(APPEND SHADER_OUTPUTS
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders/${SHADER_NAME}
    )
endforeach()

add_custom_target(copy_shaders ALL DEPENDS ${SHADER_OUTPUTS})
add_dependencies(${PROJECT_NAME} copy_shaders)

# ─── Testing ─────────────────────────────────────────────
enable_testing()

add_test(
    NAME run_engine
    COMMAND ${PROJECT_NAME}
)
