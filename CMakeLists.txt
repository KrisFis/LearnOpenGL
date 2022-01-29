
# CMake
# -----------------------------------------------------------

cmake_minimum_required(VERSION 3.10)

project(LearnOpenGL)

# Default values
# -----------------------------------------------------------

set(PROJECT_DIR ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME})
set(PROJECT_THIRD_PARTY_DIR ${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty)

# Compiler
# -----------------------------------------------------------

# Compiler
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -fpermissive")

# Paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/Binaries")
set(CMAKE_CURRENT_BINARY_DIR "${PROJECT_DIR}/Intermediate")

# Configuration types
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)

# Post fix
set(DEBUG_POSTFIX "-Debug")
set(RELEASE_POSTFIX "-Release")

# fix for MSVC lib conflict
if(MSVC)
    add_link_options("/NODEFAULTLIB:MSVCRT")
endif(MSVC)

# Definitions
# -----------------------------------------------------------

if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Win32)
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Win32)
elseif(UNIX)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Linux)
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Linux)
endif()

if(${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Debug)
    add_definitions(-D_DEBUG=1)
elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Release)
    add_definitions(-D_RELEASE=1)
endif()

# Modules
# -----------------------------------------------------------
## Should be moved to library instead of compiling itself

set(ASTD_SCRIPTS_PATH "${PROJECT_THIRD_PARTY_DIR}/ASTD/Scripts/CMake")
include(${ASTD_SCRIPTS_PATH}/ProjectFiles.cmake)

ASTD_file_public_directories(${ASTD_SCRIPTS_PATH} ASTD_INCLUDE_DIRS)

# Project Files
# -----------------------------------------------------------

include_directories(
    "${ASTD_INCLUDE_DIRS}"
    "${PROJECT_DIR}/Source/Public"
    "${PROJECT_DIR}/Source/Public/App"
    "${PROJECT_DIR}/Source/Public/Misc"
    "${PROJECT_DIR}/Source/Public/Player"
    "${PROJECT_DIR}/Source/Public/Render"
    "${PROJECT_DIR}/Source/Public/Render/Light"
    "${PROJECT_DIR}/Source/Public/Render/Light/Base"
    "${PROJECT_DIR}/Source/Public/Render/Scene"
    "${PROJECT_DIR}/Source/Public/Render/Target"
    "${PROJECT_DIR}/Source/Public/Render/Target/Base"
    "${PROJECT_DIR}/Source/Public/Texture"
    "${PROJECT_DIR}/Source/Public/TypeTraits"

    "${PROJECT_THIRD_PARTY_DIR}/OpenGL/include"
    "${PROJECT_THIRD_PARTY_DIR}/STB/include"
    "${PROJECT_THIRD_PARTY_DIR}/Assimp/include"
    "${PROJECT_THIRD_PARTY_DIR}/ImGUI/include"
)

# Sources
file(GLOB_RECURSE "PROJECT_SOURCES"
    "${PROJECT_THIRD_PARTY_DIR}/OpenGL/src/*.c"
    "${PROJECT_THIRD_PARTY_DIR}/OpenGL/src/*.cpp"
    "${PROJECT_THIRD_PARTY_DIR}/STB/src/*.cpp"
    "${PROJECT_THIRD_PARTY_DIR}/ImGUI/src/*.cpp"
    "${PROJECT_DIR}/Source/Private/*.cpp"
)

# Libraries
# -----------------------------------------------------------

set(OPENGL_LIB_ROOT ${PROJECT_THIRD_PARTY_DIR}/OpenGL/lib)
set(ASSIMP_LIB_ROOT ${PROJECT_THIRD_PARTY_DIR}/Assimp/lib)

if(WIN32)
    find_library(ASSIMP_LIB assimp HINTS "${ASSIMP_LIB_ROOT}/Win32")
    find_library(GLFW_LIB glfw3 HINTS "${OPENGL_LIB_ROOT}/GLFW/Win32")
    find_library(OPENGL_LIB opengl32)

    set(PLATFORM_LIBS "${ASSIMP_LIB}" "${GLFW_LIB}" "${OPENGL_LIB}")
elseif(UNIX)
    find_library(ASSIMP_LIB assimp)
    find_library(GLFW_LIB glfw)
    find_library(OPENGL_LIB GL)
    find_library(X11_LIB X11)

    set(PLATFORM_LIBS "${ASSIMP_LIB}" "${GLFW_LIB}" "${OPENGL_LIB}" "${X11_LIB}")
endif()

# Executable and linking
# -----------------------------------------------------------

add_executable(${PROJECT_NAME} "${PROJECT_SOURCES}")
target_link_libraries(${PROJECT_NAME} "${PLATFORM_LIBS}")

# Commands
# -----------------------------------------------------------

if(WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${PROJECT_THIRD_PARTY_DIR}/Assimp/lib/Win32/assimp-vc142-mt.dll"
            "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assimp-vc142-mt.dll"
    )
endif()