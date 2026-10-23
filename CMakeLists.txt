cmake_minimum_required(VERSION 3.20)

project(Adachi)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(
    ADACHI_PLATFORM 
    "MacOS_Desktop" 
    CACHE STRING
    "Run platform"
)
set_property(
    CACHE ADACHI_PLATFORM
    PROPERTY STRINGS MacOS_Desktop
)

# glad
add_library(glad STATIC
    vendor/glad/src/gl.c
)
target_include_directories(glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad/include
)

# glfw
add_subdirectory(vendor/glfw)

#glm
add_subdirectory(vendor/glm)

#stb
add_library(stb STATIC
    vendor/stb/stb_image.cpp
)
target_include_directories(stb PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/vendor
)

#spdlog
add_subdirectory(vendor/spdlog)

#imgui
# Dear ImGui core
add_library(imgui STATIC
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/imgui_demo.cpp
)

target_include_directories(imgui
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui"
        "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui/backends"
)

# Dear ImGui platform and renderer backends
if(ADACHI_PLATFORM STREQUAL "MacOS_Desktop")
    target_sources(imgui PRIVATE
        vendor/imgui/backends/imgui_impl_glfw.cpp
        vendor/imgui/backends/imgui_impl_opengl3.cpp
    )

    find_package(OpenGL REQUIRED)
    #find_package(glfw3 CONFIG REQUIRED)

    target_link_libraries(imgui
        PUBLIC
            glfw
            OpenGL::GL
    )

    target_compile_definitions(imgui PRIVATE
        GL_SILENCE_DEPRECATION
    )
else()
    message(FATAL_ERROR
        "ADACHI_PLATFORM must be MacOS_Desktop; got '${ADACHI_PLATFORM}'"
    )
endif()

#Assimp
include(FetchContent)

# Assimp configuration must be set before MakeAvailable().
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_FBX_IMPORTER ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "" FORCE)

# Enable this if Adachi only imports models and never exports them.
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v6.0.5
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(assimp)

# Adachi
add_library(Adachi STATIC)

# Adachi Macros
option(ADACHI_ENABLE_PROFILING "Enable Adachi profiling instrumentation" OFF)
target_compile_definitions(Adachi PUBLIC
    ADACHI_ENABLE_PROFILING=$<BOOL:${ADACHI_ENABLE_PROFILING}>
)

# Backend-independent engine code
target_sources(Adachi PRIVATE
    src/Adachi/Core/EntryPoint.cpp
    src/Adachi/Core/Application.cpp
    src/Adachi/Diagnostics/Log.cpp
    src/Adachi/Resource/ResourceManager.cpp
    src/Adachi/Resource/ResourceLoader.cpp
    src/Adachi/Renderer/RenderPipeline.cpp
    src/Adachi/Renderer/Material.cpp
    src/Adachi/Scene/Scene.cpp
    src/Adachi/Scene/Entity.cpp
)
target_include_directories(Adachi PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(Adachi PUBLIC
    ADACHI_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

# Operating-system detection
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(ADACHI_OS "macOS")

else()
    message(FATAL_ERROR
        "Unsupported operating system: '${CMAKE_SYSTEM_NAME}'"
    )
endif()

if(ADACHI_PLATFORM STREQUAL "MacOS_Desktop")
    target_sources(Adachi PRIVATE

        src/Platform/OpenGL/OpenGLBuffer.cpp
        src/Platform/OpenGL/OpenGLContext.cpp
        src/Platform/OpenGL/OpenGLRenderer.cpp
        src/Platform/OpenGL/OpenGLDebug.cpp
        src/Platform/OpenGL/OpenGLVertexArray.cpp
        src/Platform/OpenGL/OpenGLShader.cpp
        src/Platform/GLFW/GLFWCreatedWindow.cpp
        src/Platform/Factory_MacOS_Desktop.cpp
    )

    target_link_libraries(Adachi 
    PUBLIC
        glad
        glfw
        spdlog::spdlog
        glm
        imgui

    PRIVATE
        assimp::assimp
    )

else()
    message(FATAL_ERROR
        "ADACHI_PLATFORM must be MacOS_Desktop; got '${ADACHI_PLATFORM}'"
    )
endif()

add_executable(CubesApp
    examples/01-cubes/CubesApp.cpp
    examples/01-cubes/CubesScene.cpp
)
target_link_libraries(CubesApp PRIVATE
    Adachi
)

add_executable(ModelLoaderApp
    examples/02-modelloader/ModelLoaderApp.cpp
    examples/02-modelloader/ModelLoaderScene.cpp
)
target_link_libraries(ModelLoaderApp PRIVATE
    Adachi
)

