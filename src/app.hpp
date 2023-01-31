#pragma once

#include "core.hpp"
#include "common.hpp"

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