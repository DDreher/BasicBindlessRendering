#pragma once

#include "SDL_events.h"
#include "SDL_syswm.h"
#include "SDL_video.h"

class Window
{

public:
    Window(const std::string& title, uint32 width, uint32 height);
    ~Window();

    void HandleSDLEvent(const SDL_Event& sdl_event);

    void SetSize(uint32 width, uint32 height);

    uint32 GetWidth()
    {
        return width_;
    }

    uint32 GetHeight() const
    {
        return height_;
    }

    bool GetIsClosed() const
    {
        return is_closed_;
    }

    void* GetHandle();

    SDL_Window* GetSDLHandle() const
    {
        return sdl_window_handle_;
    }

private:

    std::string title_;

    uint32 pos_x_;
    uint32 pos_y_;
    uint32 width_;
    uint32 height_;

    bool is_shown_ = false;
    bool is_minimized_ = false;
    bool is_closed_ = false;

    SDL_Window* sdl_window_handle_ = nullptr;
};
