#include "Window.h"

#include "SDL_events.h"

Window::Window(const std::string& title, uint32 width, uint32 height)
    : title_(title),
    width_(width),
    height_(height)
{
    uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    sdl_window_handle_ = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, // initial position
        width_, height_,    // initial extent in pixels
        flags               // flags - see below
    );

    if (sdl_window_handle_ == nullptr)
    {
        LOG_ERROR("Failed to create window: {}", SDL_GetError());
        abort();
    }

    LOG("Window created successfully");
}

Window::~Window()
{
    CHECK(sdl_window_handle_ != nullptr);
    SDL_DestroyWindow(sdl_window_handle_);
}

void Window::HandleSDLEvent(const SDL_Event& sdl_event)
{
    if (sdl_event.type == SDL_WINDOWEVENT)
    {
        switch (sdl_event.window.event)
        {
        case SDL_WINDOWEVENT_SHOWN:
            is_shown_ = true;
            LOG("Window shown");
            break;
        case SDL_WINDOWEVENT_HIDDEN:
            is_shown_ = false;
            LOG("Window hidden");
            break;
        case SDL_WINDOWEVENT_EXPOSED:
            // Window has been exposed and should be redrawn
            break;
        case SDL_WINDOWEVENT_MOVED:
            pos_x_ = static_cast<uint32>(sdl_event.window.data1);
            pos_y_ = static_cast<uint32>(sdl_event.window.data2);
            LOG("Window moved: {}, {}", pos_x_, pos_y_);
            break;
        case SDL_WINDOWEVENT_RESIZED:
            // window has been resized
            break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            // window size has changed, either as a result of an API call or through the system or user changing the window size
            width_ = static_cast<uint32>(sdl_event.window.data1);
            height_ = static_cast<uint32>(sdl_event.window.data2);
            LOG("Window size changed: {} x {}", width_, height_);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            is_minimized_ = true;
            LOG("Window minimized");
            break;
        case SDL_WINDOWEVENT_RESTORED:
            // window has been restored to normal size and position
            LOG("Window restored");
            break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            is_minimized_ = false;
            LOG("Window maximized");
            break;
        case SDL_WINDOWEVENT_ENTER:
            // Mouse entered window
            break;
        case SDL_WINDOWEVENT_LEAVE:
            // Mouse left window
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            // Window got keyboard focus
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            // Window lost keyboard focus
            break;
        case SDL_WINDOWEVENT_CLOSE:
            LOG("Window close");
            is_closed_ = true;
            break;
        default:
            break;
        }
    }
}

void* Window::GetHandle()
{
    CHECK(sdl_window_handle_ != nullptr);

    SDL_SysWMinfo sys_info;
    SDL_VERSION(&sys_info.version); // Have to initialize the version. See: https://wiki.libsdl.org/SDL_GetWindowWMInfo
    SDL_bool success = SDL_GetWindowWMInfo(sdl_window_handle_, &sys_info);

    if (success == SDL_FALSE)
    {
        LOG_ERROR("Failed to retrieve window information: {}", SDL_GetError());
        abort();
    }

    return static_cast<void*>(sys_info.info.win.window);
}
