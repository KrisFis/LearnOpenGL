
# Setup
# -----------------------------------------------------------

cmake_minimum_required(VERSION 3.19)
project(LearnOpenGL)

# Compiler
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread -fpermissive")

# Paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/LearnOpenGL/Binaries/Linux")

# Project Files
# -----------------------------------------------------------

include_directories(
    "${CMAKE_CURRENT_LIST_DIR}/LearnOpenGL/Source/Public"
    "${CMAKE_CURRENT_LIST_DIR}/LearnOpenGL/ThirdParty/OpenGL/includes"
)

file(GLOB_RECURSE ALL_PROJECT_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/LearnOpenGL/Source/*.h" "${CMAKE_CURRENT_LIST_DIR}/LearnOpenGL/Source/*.cpp")
file(GLOB_RECURSE ALL_THIRD_PARTY_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/LearnOpenGL/Source/*.h" "${CMAKE_CURRENT_LIST_DIR}/LearnOpenGL/Source/*.cpp")

# Executable
# -----------------------------------------------------------

add_executable(LearnOpenGL ${ALL_PROJECT_FILES} ${ALL_THIRD_PARTY_FILES})

#target_link_libraries(LearnOpenGL ${OPENGL_LIBRARIES} GL m dl Xinerama Xrandr Xi Xcursor X11 Xxf86vm pthread)