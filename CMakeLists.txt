
# Setup
# -----------------------------------------------------------

cmake_minimum_required(VERSION 3.10)

project(LearnOpenGL)

# Compiler
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -fpermissive")

# Paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Binaries")
set(CMAKE_CURRENT_BINARY_DIR "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Intermediate")

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
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Unix)
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Unix)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug)
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Debug)
    add_definitions(-DDEBUG=1)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release)
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/Release)
    add_definitions(-DRELEASE=1)
endif()


# Project Files
# -----------------------------------------------------------

include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Source/Public"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Source/Public/Misc"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Source/Public/Types"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/OpenGL/include"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/STB/include"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/Assimp/include"
)

link_directories(
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/OpenGL/lib"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/Assimp/lib"
)

# Sources
file(GLOB_RECURSE "ALL_SOURCE_FILES"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/OpenGL/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/OpenGL/src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/STB/src/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Source/Private/*.cpp"
)

# Libs
set(OpenGLLibs opengl32 glfw/glfw3)
set(AssimpLibs assimp)

# Executable and linking
# -----------------------------------------------------------

add_executable(${PROJECT_NAME} "${ALL_SOURCE_FILES}")

target_link_libraries(${PROJECT_NAME} ${OpenGLLibs} ${AssimpLibs})

# Commands
# -----------------------------------------------------------

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/Assimp/lib/assimp-vc142-mt.dll"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assimp-vc142-mt.dll"
)