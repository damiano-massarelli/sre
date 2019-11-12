cmake_minimum_required(VERSION 3.6)

project(sre)

# Set C++ to use the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++ 17 for Visual Studio build
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)

#message("Project source dir: ${PROJECT_SOURCE_DIR}")
#message("Cmake home dir: ${CMAKE_HOME_DIRECTORY}")

# Searching files
file(GLOB INCLUDE_FILES
    ${PROJECT_SOURCE_DIR}/test/*.h
    ${PROJECT_SOURCE_DIR}/src/*.h
    ${PROJECT_SOURCE_DIR}/src/*/*.h
    ${PROJECT_SOURCE_DIR}/src/rendering/*/*.h
)
file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/test/*.cpp
    ${PROJECT_SOURCE_DIR}/test/*/*.cpp
    #${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/src/Engine.cpp
    ${PROJECT_SOURCE_DIR}/src/*/*.cpp
    ${PROJECT_SOURCE_DIR}/src/rendering/*/*.cpp
)

MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list *.h)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(HEADER_DIRS)

set(OTHER_LIBS_DIR "D:/software")
set(SRE_LIBS_DIR "D:/software/sre_libs")

include_directories(
    ${HEADER_DIRS}
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/external
    ${PROJECT_SOURCE_DIR}/external/nvToolsExt
    ${PROJECT_SOURCE_DIR}/external/stb_image
    ${SRE_LIBS_DIR}
    ${SRE_LIBS_DIR}/SDL2/include
    ${OTHER_LIBS_DIR}/Assimp/include
)

# Setup dependencies
link_directories(
    ${PROJECT_SOURCE_DIR}/external/SDL2/lib
    ${PROJECT_SOURCE_DIR}/external/assimp/lib
)

# GLAD
add_subdirectory(external/glad)

# Setup build
set(TARGET_NAME sre)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

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
    assimp-vc140-mt
)