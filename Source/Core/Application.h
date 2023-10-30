#pragma once
#include "Core/TickTimer.h"
#include "Core/Window.h"

class BaseApplication
{
public:
    void Run();

    static BaseApplication* Get();

    /**
     * Returns the raw timestep in ms
     */
    float GetTimestep();

    float GetFPS();

    Window* GetWindow() const { return window_; }

protected:
    virtual void Init();
    void MainLoop();
    virtual void Cleanup();

    virtual void Update();
    virtual void HandleSDLEvent(const SDL_Event& sdl_event);
    virtual void Render();
    virtual void RenderUI();

    void InitWindow();
    void DestroyWindow();

    std::string application_name_;
    Window* window_ = nullptr;
    TickTimer tick_timer_;

private:
    static inline BaseApplication* instance_ = nullptr;
};
