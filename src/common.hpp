#pragma once

#define mouse_down(b) (input.buttons[b].down && input.buttons[b].changed)

namespace WinGameAlpha{

struct Mouse_State{
    int x_pos = 0;
    int y_pos = 0;
    bool down = false;
    bool changed = false;
};

struct Input {
    Mouse_State mouse_state;
};

}