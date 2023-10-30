#pragma once
#include "SDL_events.h"
#include "SDL_keycode.h"

namespace input
{
    enum Button : int32
    {
        INVALID = -1,
        MOUSE_LEFT = 1,
        MOUSE_MIDDLE = 2,
        MOUSE_RIGHT = 3,
        NUM
    };

    void BeginNewFrame();
    void HandleSDLEvent(const SDL_Event& sdl_event);

    bool IsKeyDown(const SDL_Keycode& key_code);
    bool IsKeyUp(const SDL_Keycode& key_code);
    bool IsKeyReleased(const SDL_Keycode& key_code);

    void ResetMousePosDelta();
    Vec2 GetMousePos();
    Vec2 GetMousePosDelta();
    bool IsButtonDown(Button button);
    void SetCursorVisibility(bool is_cursor_visible);
}