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

render_rect_properties arena_rect = {
    .width = ARENA_R*2,
    .height = ARENA_U*2,
    .colour = ARENA_COLOUR
};
render_rect_properties player_rect = {
    .width = P_WIDTH,
    .height = P_HEIGHT,
    .colour = P_COLOUR
};
render_rect_properties ball_rect = {
    .width = B_DIAMETER,
    .height = B_DIAMETER,
    .colour = B_COLOUR
};

render_rect_properties player_rect_v2[] = {
    {
        .width=P_WIDTH, .height=P_HEIGHT, .colour = P_COLOUR //fill
    },
    {
        .y_offset=P_HEIGHT/2+P_BORDER_WIDTH, .width=P_WIDTH, .height=P_BORDER_WIDTH //top
    },
    {
        .y_offset=-P_HEIGHT/2-P_BORDER_WIDTH, .width=P_WIDTH, .height=P_BORDER_WIDTH //bottom
    },
    {
        .x_offset=-P_WIDTH/2-P_BORDER_WIDTH, .width=P_BORDER_WIDTH, .height=P_HEIGHT //left
    }
};

}