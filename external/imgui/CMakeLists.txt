cmake_minimum_required(VERSION 3.6)
project(imgui)

add_library(
    imgui STATIC
    imconfig.h
    imgui.h imgui.cpp
    imgui_draw.cpp
    imgui_impl_opengl3.h imgui_impl_opengl3.cpp
    imgui_impl_sdl.h imgui_impl_sdl.cpp
    imgui_impl_win32.h imgui_impl_win32.cpp
    imgui_internal.h
    imgui_widgets.cpp
    imstb_rectpack.h imstb_textedit.h imstb_truetype.h
)
target_link_libraries(imgui glad SDL2)