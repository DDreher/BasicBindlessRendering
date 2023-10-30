#include "Input.h"

namespace
{
    Vec2 mouse_pos = Vec2::ZERO;
    Vec2 mouse_pos_delta = Vec2::ZERO;
    std::unordered_map<SDL_Keycode, bool> keys_pressed;
    std::unordered_map<SDL_Keycode, bool> prev_keys_pressed;
    std::unordered_map<input::Button, bool> buttons_pressed;
}

void input::BeginNewFrame()
{
    prev_keys_pressed = keys_pressed;
}

void input::HandleSDLEvent(const SDL_Event& sdl_event)
{
    if(sdl_event.type == SDL_KEYDOWN)
    {
        keys_pressed[sdl_event.key.keysym.sym] = true;
        return;
    }

    if (sdl_event.type == SDL_KEYUP)
    {
        keys_pressed[sdl_event.key.keysym.sym] = false;
        return;
    }

    if (sdl_event.type == SDL_MOUSEBUTTONDOWN)
    {
        input::Button button = (Button) sdl_event.button.button;
        buttons_pressed[button] = true;
        return;
    }

    if (sdl_event.type == SDL_MOUSEBUTTONUP)
    {
        input::Button button = (Button)sdl_event.button.button;
        buttons_pressed[button] = false;
        return;
    }

    if (sdl_event.type == SDL_MOUSEMOTION)
    {
        mouse_pos = { (float) sdl_event.motion.x, (float) sdl_event.motion.y };
        mouse_pos_delta = { (float) sdl_event.motion.xrel, (float) sdl_event.motion.yrel };
        return;
    }
}

bool input::IsKeyDown(const SDL_Keycode& key_code)
{
    return keys_pressed[key_code];
}

bool input::IsKeyUp(const SDL_Keycode& key_code)
{
    return !keys_pressed[key_code];
}

bool input::IsKeyReleased(const SDL_Keycode& key_code)
{
    return prev_keys_pressed[key_code] && !keys_pressed[key_code];
}

bool input::IsButtonDown(Button button)
{
    return buttons_pressed[button];
}

void input::ResetMousePosDelta()
{
    mouse_pos_delta = Vec2::ZERO;
}

Vec2 input::GetMousePos()
{
    return mouse_pos;
}

Vec2 input::GetMousePosDelta()
{
    return mouse_pos_delta;
}

void input::SetCursorVisibility(bool is_cursor_visible)
{
    SDL_SetRelativeMouseMode(is_cursor_visible ? SDL_FALSE : SDL_TRUE);
}
