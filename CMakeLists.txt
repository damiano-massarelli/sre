cmake_minimum_required(VERSION 3.6)

project(sre)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Set C++ to use the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++ 17 for Visual Studio build
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

#message("Cmake home dir: ${CMAKE_HOME_DIRECTORY}")

# Searching files
file(GLOB INCLUDE_FILES
    ${PROJECT_SOURCE_DIR}/test/*.h
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*/*.h
    ${PROJECT_SOURCE_DIR}/src/rendering/*/*.h
)
file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/Application.cpp
    ${PROJECT_SOURCE_DIR}/test/*.cpp
    ${PROJECT_SOURCE_DIR}/test/*/*.cpp
    ${PROJECT_SOURCE_DIR}/src/Engine.cpp
    ${PROJECT_SOURCE_DIR}/src/*/*.cpp
    ${PROJECT_SOURCE_DIR}/src/rendering/*/*.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/external
    ${PROJECT_SOURCE_DIR}/external/KHR
    ${PROJECT_SOURCE_DIR}/external/glm
    ${PROJECT_SOURCE_DIR}/external/nvToolsExt
    ${PROJECT_SOURCE_DIR}/external/stb_image
    ${PROJECT_SOURCE_DIR}/external/SDL2/include
    ${PROJECT_SOURCE_DIR}/external/assimp/include
)

# Setup dependencies
link_directories(
    ${PROJECT_SOURCE_DIR}/external/SDL2/lib/x64
    ${PROJECT_SOURCE_DIR}/external/assimp/lib
)

# Glad
add_subdirectory(external/glad)

# ImGui
add_subdirectory(external/imgui)

# Assimp
add_subdirectory(external/assimp ${EXECUTABLE_OUTPUT_PATH}/external/assimp)

# Assimp
add_subdirectory(external/SDL2 ${EXECUTABLE_OUTPUT_PATH}/external/SDL2)

# Setup build
set(TARGET_NAME sre)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

add_executable(${TARGET_NAME} ${SRC_FILES})

set_target_properties(sre PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)

target_link_libraries(
    ${TARGET_NAME}
    opengl32
    SDL2Main
    SDL2
    glad
    imgui
    assimp
)