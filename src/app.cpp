
#include "app.hpp"
#include "textures.hpp"
#include "render_objects.hpp"
#include "texture_manager.hpp"
#include "renderer.hpp"

#define CANVAS_UNIT_SIZE 5.f
#define CANVAS_WIDTH 180.f
#define CANVAS_HEIGHT 90.f
#define CANVAS_MATRIX_WIDTH CANVAS_WIDTH/CANVAS_UNIT_SIZE
#define CANVAS_MATRIX_HEIGHT CANVAS_HEIGHT/CANVAS_UNIT_SIZE

namespace WinGameAlpha {

// Globals
extern bool running;
extern bool updates;

uint32_t* canvas_matrix;

shared_ptr<Drawer> drawer;
shared_ptr<Texture_Manager> texture_manager;
shared_ptr<Render_Matrix> canvas;

// Functions

void app_cleanup(){
    VirtualFree(canvas_matrix,0,MEM_RELEASE);
}

inline void memset32(void *m, uint32_t val, size_t count)
{
    uint32_t *buf = (uint32_t*)m;
    while(count--) *buf++ = val;
}

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
    shared_ptr<Render_Matrix> m1 = texture_manager->create_render_matrix(0,0,2,5,test_matrix,10,10);
    texture_manager->create_render_object(m1,0);
    shared_ptr<Render_Matrix> m2 = texture_manager->create_render_matrix(0,0,1,10,test_matrix,10,10);
    texture_manager->create_render_object(m2,1);
    int canvas_width = floor(CANVAS_MATRIX_WIDTH);
    int canvas_height = floor(CANVAS_MATRIX_HEIGHT);
    canvas_matrix = (uint32_t*)VirtualAlloc(0,canvas_height*canvas_width*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memset32(canvas_matrix,AB,canvas_height*canvas_width);
    canvas_matrix[30] = 0xffff00;
    canvas_matrix[50] = 0xff0000;
    canvas_matrix[60] = 0xff0000;
    canvas = texture_manager->create_render_matrix(0,0,(float)canvas_width,(float)canvas_height,canvas_matrix,CANVAS_UNIT_SIZE,CANVAS_UNIT_SIZE);
    texture_manager->create_render_object(canvas,1);
    WGAERRCHECK(texture_manager->register_all_objects());
    drawer->set_background_colour(BACKGROUND_COLOUR);
}

void render_update(){
#ifdef USING_OPENCL
    WGAERRCHECK(drawer->cl_resize());
#endif
}

void render_tick(Input& input, float dt){
    if (!updates) return;
    cout << "UPDATED" << endl;
    drawer->draw_objects();
}

}