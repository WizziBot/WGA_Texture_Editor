#include "app.hpp"
#include "textures.hpp"

namespace WinGameAlpha {

// Globals
extern bool running;
extern bool updates;

shared_ptr<Drawer> drawer;
vector<shared_ptr<Render_Object>> render_objects;

// Functions

void render_init(){
    wga_err err;

    // Init drawer
    drawer = make_shared<Drawer>(err);
    if (err != WGA_SUCCESS){
        drawer.reset();
        WGACHECKERRNO("Failed to instantiate drawer.",err);
        return;
    }
    // Register objects
    // shared_ptr<Render_Object> arena_robj = make_shared<Render_Object>(drawer, &arena_rect,1,ARENA_RENDER_LAYER,false);
    // render_objects.push_back(arena_robj);
    shared_ptr<Render_Object> player_robj = make_shared<Render_Object>(drawer, &player_render_matrix,0);
    render_objects.push_back(player_robj);
    
    drawer->set_background_colour(BACKGROUND_COLOUR);
    drawer->draw_objects();
}

void render_update(){
    
}

void render_tick(Input& input, float dt){
    if (!updates) return;
    cout << "UPDATED" << endl;
    drawer->draw_objects();
}

}