
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

void process_mouse_down(int mouse_x, int mouse_y){
    // Convert to abs coords
    float factor = (float)render_state.height/100.f;
    int canvas_width = floor(CANVAS_WIDTH*factor);
    int canvas_horz_border = floor((render_state.width-canvas_width)/2);
    int canvas_height = floor(CANVAS_HEIGHT*factor);
    int canvas_vert_border = floor((render_state.height-canvas_height)/2);
    int canvas_unit_size = floor(CANVAS_UNIT_SIZE*factor);
    // Find canvas matrix square
    if (within_bounds(canvas_horz_border,mouse_x,canvas_width+canvas_horz_border) && within_bounds(canvas_vert_border,mouse_y,canvas_height+canvas_vert_border)){
        int matrix_x = (mouse_x-canvas_horz_border)/canvas_unit_size;
        int matrix_y = (mouse_y-canvas_vert_border)/canvas_unit_size;
        int matrix_index = matrix_x + CANVAS_MATRIX_WIDTH*matrix_y;
        canvas_matrix[matrix_index] = 0xff0000; //draw red
        updates = true;
    }
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
    // shared_ptr<Render_Matrix> m1 = texture_manager->create_render_matrix(0,0,2,5,test_matrix,10,10);
    // texture_manager->create_render_object(m1,0);
    // shared_ptr<Render_Matrix> m2 = texture_manager->create_render_matrix(0,0,1,10,test_matrix,10,10);
    // texture_manager->create_render_object(m2,1);
    // WGAERRCHECK(texture_manager->save_texture(test_matrix,10,"test.wgat"));
    
    WGAERRCHECK(texture_manager->load_texture(test_matrix10,10,"test.wgat"));
    shared_ptr<Render_Matrix> m2 = texture_manager->create_render_matrix(0,0,2,5,test_matrix10,10,10);
    texture_manager->create_render_object(m2,0);

    int canvas_width = floor(CANVAS_MATRIX_WIDTH);
    int canvas_height = floor(CANVAS_MATRIX_HEIGHT);
    canvas_matrix = (uint32_t*)VirtualAlloc(0,canvas_height*canvas_width*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memset32(canvas_matrix,AB,canvas_height*canvas_width);

    canvas = texture_manager->create_render_matrix(0,0,(float)canvas_width,(float)canvas_height,canvas_matrix,CANVAS_UNIT_SIZE,CANVAS_UNIT_SIZE);
    texture_manager->create_render_object(canvas,0);
    WGAERRCHECK(texture_manager->register_all_objects());
    drawer->set_background_colour(BACKGROUND_COLOUR);
}

void render_update(){
#ifdef USING_OPENCL
    WGAERRCHECK(drawer->cl_resize());
#endif
}

void render_tick(Input& input, float dt){
    if (mouse_clicked()){
        input.mouse_state.changed = false;
    }
    if (mouse_down()) {
        // Normalize coordinates
        int mouse_x = input.mouse_state.x_pos;
        int mouse_y = render_state.height - input.mouse_state.y_pos;
        process_mouse_down(mouse_x,mouse_y);
    }
    if (mouse_released()){
        input.mouse_state.changed = false;
    }

    if (!updates) return;
    drawer->draw_objects();
}

}