cmake_minimum_required(VERSION 3.8)

# Clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

### MACRO for sort file inside a project in VS
MACRO(SOURCE_GROUP_BY_FOLDER target)
  SET(SOURCE_GROUP_DELIMITER "/")
  SET(last_dir "")
  SET(files "")
  FOREACH(file ${${target}_SRC})
    file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}" ${file})
    GET_FILENAME_COMPONENT(dir "${relative_file}" PATH)
    IF (NOT "${dir}" STREQUAL "${last_dir}")
      IF (files)
        SOURCE_GROUP("${last_dir}" FILES ${files})
      ENDIF (files)
      SET(files "")
    ENDIF (NOT "${dir}" STREQUAL "${last_dir}")
    SET(files ${files} ${file})
    SET(last_dir "${dir}")
  ENDFOREACH(file)
  IF (files)
    SOURCE_GROUP("${last_dir}" FILES ${files})
  ENDIF (files)
ENDMACRO(SOURCE_GROUP_BY_FOLDER)

# Setup target
set(TARGET sre)

project(${TARGET})

# Turn on the ability to create folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH})

# Set C++ to use the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Debug macros
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSRE_DEBUG")

# C++ 17 for Visual Studio build
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

file(GLOB_RECURSE ${TARGET}_SRC
    ${PROJECT_SOURCE_DIR}/Application.cpp
    ${PROJECT_SOURCE_DIR}/test/*.cpp
    ${PROJECT_SOURCE_DIR}/test/*.h
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/test
    ${PROJECT_SOURCE_DIR}/external
    ${PROJECT_SOURCE_DIR}/external/KHR
    ${PROJECT_SOURCE_DIR}/external/glm
    ${PROJECT_SOURCE_DIR}/external/nvToolsExt
    ${PROJECT_SOURCE_DIR}/external/stb_image
)

# Glad
add_subdirectory(external/glad ${EXECUTABLE_OUTPUT_PATH}/external/glad)

# Assimp
add_subdirectory(external/assimp ${EXECUTABLE_OUTPUT_PATH}/external/assimp)
include_directories(${PROJECT_SOURCE_DIR}/external/assimp/include)

# SDL2
add_subdirectory(external/SDL2 ${EXECUTABLE_OUTPUT_PATH}/external/SDL2)
include_directories(${PROJECT_SOURCE_DIR}/external/SDL2/include)

# ImGui
add_subdirectory(external/imgui ${EXECUTABLE_OUTPUT_PATH}/external/imgui)

# Setup dependencies
link_directories(
    ${EXECUTABLE_OUTPUT_PATH}/external/glad
    ${EXECUTABLE_OUTPUT_PATH}/external/imgui
    ${EXECUTABLE_OUTPUT_PATH}/external/SDL2
    ${EXECUTABLE_OUTPUT_PATH}/external/assimp
)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${TARGET})

add_executable(${TARGET} ${${TARGET}_SRC})

# Group sources into folders for VS
SOURCE_GROUP_BY_FOLDER(${TARGET})

set_target_properties(sre PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")

target_link_libraries(
    ${TARGET}
    opengl32
    SDL2Main
    SDL2
    glad
    imgui
    assimp
)