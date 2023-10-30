#include "Core/Application.h"

#include "SDL.h"

#include "Core/Input.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/IRenderer.h"

void BaseApplication::Run()
{
    Init();
    MainLoop();
    Cleanup();
}

BaseApplication* BaseApplication::Get()
{
    return instance_;
}

float BaseApplication::GetTimestep()
{
    return (float)tick_timer_.elapsed_;
}

float BaseApplication::GetFPS()
{
    return 1.0f / GetTimestep();
}

void BaseApplication::Init()
{
    CHECK(instance_ == nullptr);
    LOG("Initializing application: {}", application_name_);
    instance_ = this;

    SDL_Init(SDL_INIT_VIDEO);
    InitWindow();

    gfx::Init(window_);
}

void BaseApplication::MainLoop()
{
    LOG("Entering Main Loop...");

    while (window_ != nullptr && window_->GetIsClosed() == false)
    {
        tick_timer_.Update();
        Update();
        Render();

        if (input::IsKeyDown(SDL_KeyCode::SDLK_ESCAPE))
        {
            DestroyWindow();
        }
    }
}

void BaseApplication::Cleanup()
{
    LOG("Tearing down application...");

    gfx::Shutdown();
    DestroyWindow();
    SDL_Quit();
}

void BaseApplication::InitWindow()
{
    window_ = new Window(application_name_, 1920, 1080);
    CHECK(window_ != nullptr);
}

void BaseApplication::DestroyWindow()
{
    if (window_ != nullptr)
    {
        delete window_;
        window_ = nullptr;
    }
}

void BaseApplication::Update()
{
    input::BeginNewFrame();
    input::ResetMousePosDelta();    // Have to manually reset, otherwise we only update on mouse moved event.

    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event))
    {
        HandleSDLEvent(sdl_event);
    }
}

void BaseApplication::HandleSDLEvent(const SDL_Event& sdl_event)
{
    input::HandleSDLEvent(sdl_event);

    if(window_ != nullptr)
    {
        window_->HandleSDLEvent(sdl_event);
    }
}

void BaseApplication::Render()
{
    gfx::renderer->Render();
    gfx::renderer->Present();
}

void BaseApplication::RenderUI()
{
}
