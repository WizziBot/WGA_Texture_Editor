#pragma once
#include "app.hpp"

#define BACKGROUND_COLOUR 0x2A2A2A
#define ARENA_COLOUR 0x222222
#define B_COLOUR 0xffffff
#define P_COLOUR 0xf80505
#define P_BORDER_COLOUR 0x991111
#define P_BORDER_WIDTH 2

// struct render_rect_properties{
//     float x_offset=0;
//     float y_offset=0;
//     float width;
//     float height;
//     uint32_t colour;
// };

namespace WinGameAlpha {

// notice that player collider height will need to be adjusted to this
uint32_t player_matrix[] {
    0xff0000,
    0xff0000,
    0xff0000,
    0xff0000,
    0xff0000
};

Render_Matrix player_render_matrix(0,0,1,5,player_matrix,4,4);

}