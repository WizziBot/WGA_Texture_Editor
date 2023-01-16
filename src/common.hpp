#pragma once

#define NUM_0 0x30
#define NUM_1 0x31
#define NUM_2 0x32
#define NUM_3 0x33
#define NUM_4 0x34
#define NUM_5 0x35
#define NUM_6 0x36
#define NUM_7 0x37
#define NUM_8 0x38
#define NUM_9 0x39
#define VK_S 0x53

// Modulo 0x30 will give key

#define btn_down(b) input.buttons[b].down
#define btn_pressed(b) (input.buttons[b].down && input.buttons[b].changed)
#define btn_released(b) (!input.buttons[b].down && input.buttons[b].changed)

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

struct Button_State{
    bool down = false;
    bool changed = false;
};

enum Button_Type{
    BUTTON_CTRL_S = 10,

    BUTTON_COUNT // Relys on enum 0 based incremented
};

struct Input {
    Button_State buttons[BUTTON_COUNT];
    Mouse_State mouse_state;
};

}