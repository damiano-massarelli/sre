#pragma once

#include "events/EventListener.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>

#include <functional>


class UIRenderer : public EventListener
{
public:
    UIRenderer() = default;

    /**
     *  This will create the render context for the UI and set the initial settings
     */
    void init();

    /**
     *  Render the UI calling all the registered draw functions
     *  TODO: Set the default to false and decide whether or not in the render system render function
     */
    void render(bool showDebugUI = true);

    /**
     *  Clear the registered draw functions
     */
    void cleanUp();

    /**
     *  Perform a cleanup and destroys the UI rendering context
     */
    void shutdown();

    /**
     *  Add a new UI drawer.
     *
     * \param   drawer  A pointer to the function that will draw the UI
     */
    void addUIDrawer(std::function<void()> drawer);

    // TODO: Remove drawer
    
    /**
     *  To use for UI that is for debug purposes (e.g. FPS counter)
     *  This will not be cleaned when cleanUp() is called.
     *  It will be visible until shutdown() is called.
     *
     * \param   drawer  A pointer to the function that will draw the UI
     */
    void setDebugUIDrawer(std::function<void()> drawer);

    // EventListener implementations
    virtual void onEvent(SDL_Event e) override;

private:
    struct SDL_Window* mWindow;
    
    std::function<void()> mDebugUIDrawer = nullptr;

    std::vector<std::function<void()>> UIDrawers;
};