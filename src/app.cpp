
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
uint32_t* loaded_texture;
int cv_lower_x=0, cv_lower_y=0;
int cv_higher_x=0, cv_higher_y=0;
int canvas_width;
int canvas_height;
shared_ptr<Drawer> drawer;
shared_ptr<Texture_Manager> texture_manager;
shared_ptr<Render_Matrix> canvas;

const char load_texture_name[] = "texture.wgat";

// Functions

void app_cleanup(){
    uint32_t* submatrix = texture_manager->crop_matrix(canvas_matrix,canvas_width,cv_lower_x,cv_lower_y,cv_higher_x,cv_higher_y);
    if (submatrix != NULL){
        cout << "Width: " << cv_higher_x-cv_lower_x+1 << " Height: " << cv_higher_y-cv_lower_y+1 << endl;
        wga_err err = texture_manager->save_texture(submatrix,(cv_higher_x-cv_lower_x+1),(cv_higher_y-cv_lower_y+1),load_texture_name);
        if (err == WGA_SUCCESS) cout << "Success: Texture saved" << endl;
        else cerr << "Error writing to file: Texture not saved" << endl;
        VirtualFree(submatrix,0,MEM_RELEASE);
    } else {
        cerr << "Unable to generate submatrix: Texture not saved" << endl;
    }
    VirtualFree(canvas_matrix,0,MEM_RELEASE);
    VirtualFree(loaded_texture,0,MEM_RELEASE);
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

        if (matrix_x < cv_lower_x) cv_lower_x = matrix_x;
        if (matrix_y < cv_lower_y) cv_lower_y = matrix_y;
        if (matrix_x > cv_higher_x) cv_higher_x = matrix_x;
        if (matrix_y > cv_higher_y) cv_higher_y = matrix_y;

        canvas_matrix[matrix_index] = 0x67cdfd; //draw red
        updates = true;
    }
}

void load_onto_canvas(uint32_t* matrix, int width, int height){
    int x0 = canvas_width/2 - width/2;
    int y0 = canvas_height/2 - height/2;
    int x1 = x0 + width;
    int y1 = y0 + height;
    uint32_t* unit = canvas_matrix;
    uint32_t* src = matrix;
    for (int y=y0; y < y1; y++){
        unit = canvas_matrix + x0 + y*canvas_width;
        for (int x=x0; x < x1; x++){
            *unit++ = *src;
            src++;
        }
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
    // Setup canvas
    canvas_width = floor(CANVAS_MATRIX_WIDTH);
    canvas_height = floor(CANVAS_MATRIX_HEIGHT);
    cv_higher_x = cv_lower_x = canvas_width/2;
    cv_higher_y = cv_lower_y = canvas_height/2;
    canvas_matrix = (uint32_t*)VirtualAlloc(0,canvas_height*canvas_width*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memset32(canvas_matrix,AB,canvas_height*canvas_width);

    // Load texture
    int width,height;
    err = texture_manager->load_texture(&loaded_texture,&width,&height,load_texture_name);
    if (err == WGA_SUCCESS){
        // shared_ptr<Render_Matrix> m2 = texture_manager->create_render_matrix(0,0,width,height,loaded_texture,5,5);
        // texture_manager->create_render_object(m2,0);
        load_onto_canvas(loaded_texture,width,height);
        cout << "Loaded Texture: " << load_texture_name << endl;
    } else cout << "No texture loaded" << endl;

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