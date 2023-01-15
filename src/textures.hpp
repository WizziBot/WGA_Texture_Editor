#pragma once
#include "app.hpp"

#define BACKGROUND_COLOUR 0x252526
#define ARENA_COLOUR 0x1e1e1e
#define B_COLOUR 0xffffff
#define P_COLOUR 0xf80505

namespace WinGameAlpha {

// notice that player collider height will need to be adjusted to this
uint32_t player_matrix[] {
    0xff0000,
    0xff5500,
    0xff0055,
    0xffff00,
    0xff00ff
};
uint32_t test_matrix[] {
    0xffffff,0x664433,
    0xff5500,0x000000,
    0xff0055,0x118800,
    0xffff00,0x110066,
    0xff00ff,0x0000ff
};

uint32_t test_matrix2[] {
    0xffffff,0x555555, 0xff00ff,
    0x00ff00,0x0000ff, 0x555555,
    0x555555,0x888888, 0xffffff
};
uint32_t test_matrix3[] {
    0xffffff,0xf5f5f5, 0xf2f2f2,
    0xe5e5e5,0xff99ff, 0xf5f5f5,
    0xe5e5e5,0xffffff, 0xf2f2f2
};

uint32_t arena_matrix[] {
    B_COLOUR
};

uint32_t test_matrix10[10] = {0};

}