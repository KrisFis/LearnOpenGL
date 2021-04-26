
# Setup
# -----------------------------------------------------------

cmake_minimum_required(VERSION 3.10)

project(LearnOpenGL)

# Compiler
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -fpermissive")

# Paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Binaries/${CMAKE_SYSTEM_NAME}")

# Configuration types
set(CMAKE_CONFIGURATION_TYPES Debug Release FORCE )

# Post fix
set(DEBUG_POSTFIX "-Debug")
set(RELEASE_POSTFIX "-Release")

# fix for MSVC lib conflict
if(MSVC)
    add_link_options("/NODEFAULTLIB:MSVCRT")
endif(MSVC)

# Project Files
# -----------------------------------------------------------

include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Source/Public"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/OpenGL/include"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/LodePNG/include"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/JPEGCompressor/include"
)

link_directories(
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/OpenGL/lib"
)

# Sources
file(GLOB_RECURSE "ALL_SOURCE_FILES"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/Source/Private/*.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/*/src/*.c"
    "${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}/ThirdParty/*/src/*.cpp"
)

# Libs
set(OpenGLLibs opengl32 GLFW/glfw3)

# Executable and linking
# -----------------------------------------------------------

add_executable(${PROJECT_NAME} "${ALL_SOURCE_FILES}")

target_link_libraries(${PROJECT_NAME} ${OpenGLLibs})