#pragma once

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

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