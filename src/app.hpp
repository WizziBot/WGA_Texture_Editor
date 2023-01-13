#pragma once

#include "core.hpp"
#include "common.hpp"

// Rendering
#define ARENA_RENDER_LAYER 0
#define P_B_RENDER_LAYER 1

// Size defines
#define ARENA_R 90.f
#define ARENA_U 45.f
#define P_WIDTH 5
#define P_HEIGHT 24
#define B_DIAMETER 2
#define P_X_DISPLACEMENT 82


namespace WinGameAlpha{

extern Render_State render_state;

/* Render on window start */
void render_init();
/* Render on gameloop cycle */
void render_tick(Input& input, float dt);
/* Render on resize event */
void render_update();

void app_cleanup();
}