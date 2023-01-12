
#include "textures.hpp"
#include "app.hpp"

namespace WinGameAlpha {

// Globals
extern bool running;
extern bool updates;

shared_ptr<Drawer> drawer;
shared_ptr<Texture_Manager> texture_manager;

// Functions

void render_init(){
    wga_err err;

    // Init drawer
    drawer = make_shared<Drawer>(&err);
    if (err != WGA_SUCCESS){
        WGACHECKERRNO("Failed to instantiate drawer.",err);
        return;
    }
    texture_manager = make_shared<Texture_Manager>(drawer);
    if (err != WGA_SUCCESS){
        WGACHECKERRNO("Failed to instantiate texture_manager.",err);
        return;
    }

    // Register objects
    // shared_ptr<Render_Object> arena_robj = make_shared<Render_Object>(drawer, &arena_rect,1,ARENA_RENDER_LAYER,false);
    // render_objects.push_back(arena_robj);
    Render_Matrix* m1 = texture_manager->create_render_matrix(0,0,2,5,test_matrix,10,10);
    texture_manager->create_render_object(m1,0);

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