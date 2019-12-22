#pragma once

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>
#include <functional>

class UIRenderer
{
public:
    UIRenderer() = default;

    void init();
    void render();
    void cleanUp();
    
    void setDebugUIDrawer(std::function<void()> drawer);

private:
    bool mIsDebugUI = false;
    struct SDL_Window* mWindow;
    
    std::function<void()> mDebugUIDrawer = nullptr;
    
    void initDebugUI();
    void renderDebugUI();
};