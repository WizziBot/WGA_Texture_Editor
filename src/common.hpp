#pragma once

#define mouse_down() input.mouse_state.down
#define mouse_clicked() (input.mouse_state.down && input.mouse_state.changed)
#define mouse_released() (!input.mouse_state.down && input.mouse_state.changed)

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