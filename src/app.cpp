
#include "app.hpp"
#include "textures.hpp"
#include "render_objects.hpp"
#include "texture_manager.hpp"
#include "renderer.hpp"
#include "settings_parser.hpp"
#include <filesystem>

namespace fs = std::filesystem;

// #define CANVAS_UNIT_SIZE 5.f
#define CANVAS_WIDTH 175.f
#define CANVAS_HEIGHT 90.f
// #define CANVAS_MATRIX_WIDTH CANVAS_WIDTH/CANVAS_UNIT_SIZE
// #define CANVAS_MATRIX_HEIGHT CANVAS_HEIGHT/CANVAS_UNIT_SIZE


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
float canvas_unit_size;
float canvas_matrix_width;
int colours_size;
float cv_offsetX;
float cv_offsetY;
uint32_t active_colour = 0;
shared_ptr<Drawer> drawer;
shared_ptr<Texture_Manager> texture_manager;
shared_ptr<Render_Matrix> canvas;
shared_ptr<App_Settings> settings;
bool first_stroke = true;

string load_texture_name;

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
    int cv_unit_size = floor(canvas_unit_size*factor);
    int cv_width = cv_unit_size*canvas_width;
    int cv_height = cv_unit_size*canvas_height;
    int canvas_horz_border = floor((render_state.width-cv_width)/2 + cv_offsetX);
    int canvas_vert_border = floor((render_state.height-cv_height)/2 + cv_offsetY);
    if (within_bounds(canvas_horz_border,mouse_x,cv_width+canvas_horz_border) && within_bounds(canvas_vert_border,mouse_y,cv_height+canvas_vert_border)){
        // Find canvas matrix square
        int matrix_x = (mouse_x-canvas_horz_border)/cv_unit_size;
        int matrix_y = (mouse_y-canvas_vert_border)/cv_unit_size;
        int matrix_index = matrix_x + canvas_matrix_width*matrix_y;
        
        // Adjust submatrix boundary
        if ((cv_higher_x-cv_lower_x == 0 || cv_higher_y-cv_lower_y == 0) && first_stroke){
            cv_higher_x = cv_lower_x = matrix_x;
            cv_higher_y = cv_lower_y = matrix_y;
            first_stroke = false;
        } 
        if (active_colour != ALPHA_BIT){
            if (matrix_x < cv_lower_x) cv_lower_x = matrix_x;
            if (matrix_y < cv_lower_y) cv_lower_y = matrix_y;
            if (matrix_x > cv_higher_x) cv_higher_x = matrix_x;
            if (matrix_y > cv_higher_y) cv_higher_y = matrix_y;
        }

        canvas_matrix[matrix_index] = active_colour;
        updates = true;
    }
}

wga_err get_load_texture_name() {
    for (auto &p : fs::recursive_directory_iterator()) {
        if (p.path().extension() == ".wgat"){
            load_texture_name = p.path().generic_string();
            return WGA_SUCCESS;
        }
    }
    return WGA_FAILURE;
}

void save_canvas(){
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
}

void load_onto_canvas(uint32_t* matrix, int width, int height){
    int x0 = floor((float)canvas_width/2.f - (float)width/2.f);
    int y0 = floor((float)canvas_height/2.f - (float)height/2.f);
    int x1 = x0 + width;
    int y1 = y0 + height;
    cv_lower_x = x0;
    cv_higher_x = x1;
    cv_lower_y = y0;
    cv_higher_y = y1;
    uint32_t* unit = canvas_matrix;
    uint32_t* src = matrix;
    for (int y=y0; y < y1; y++){
        unit = canvas_matrix + x0 + y*canvas_width;
        for (int x=x0; x < x1; x++){
            *unit++ = *src;
            src++;
        }
    }
    VirtualFree(matrix,0,MEM_RELEASE);
}

void render_init(){
    wga_err err;

    settings = make_shared<App_Settings>(&err);
    if (err != WGA_SUCCESS){
        WGACHECKERRNO("Failed to load settings.",err);
        return;
    }
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
    // Canvas background
    shared_ptr<Render_Matrix> canvas_background = texture_manager->create_render_matrix(0,0,1,1,background_matrix,CANVAS_WIDTH,CANVAS_HEIGHT);
    texture_manager->create_render_object(canvas_background,0);

    // Setup canvas
    colours_size = settings->get_colours_size();
    canvas_unit_size = settings->get_unit_size();
    canvas_matrix_width = CANVAS_WIDTH/canvas_unit_size;
    canvas_width = floor(canvas_matrix_width);
    canvas_height = floor(CANVAS_HEIGHT/canvas_unit_size);
    cv_higher_x = cv_lower_x = canvas_width/2;
    cv_higher_y = cv_lower_y = canvas_height/2;
    canvas_matrix = (uint32_t*)VirtualAlloc(0,canvas_height*canvas_width*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memset32(canvas_matrix,AB,canvas_height*canvas_width);
    canvas_matrix[34] = 0;
    // Load texture
    int width,height;
    if (get_load_texture_name() == WGA_FAILURE) load_texture_name = "texture.wgat";

    err = texture_manager->load_texture(&loaded_texture,&width,&height,load_texture_name);
    if (err == WGA_SUCCESS){
        load_onto_canvas(loaded_texture,width,height); // not to self: also fix cv_higher/lower stuff when loaded
        cout << "Loaded Texture: " << load_texture_name << endl;
    } else cout << "No texture loaded" << endl;
    float factor = (float)render_state.height/100.f;
    cv_offsetX = fmod(CANVAS_WIDTH*factor,canvas_unit_size*factor)/2;
    cv_offsetY = fmod(CANVAS_HEIGHT*factor,canvas_unit_size*factor)/2;
    canvas = texture_manager->create_render_matrix(cv_offsetX,cv_offsetY,(float)canvas_width,(float)canvas_height,canvas_matrix,canvas_unit_size,canvas_unit_size);
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
    if (mouse_down()) {
        // Normalize coordinates
        int mouse_x = input.mouse_state.x_pos;
        int mouse_y = render_state.height - input.mouse_state.y_pos;
        process_mouse_down(mouse_x,mouse_y);
    }
    if (btn_pressed(BUTTON_CTRL_S)) {
        save_canvas();
    }
    for (int i=0; i < colours_size; i++){
        if (btn_down(i)){
            active_colour = settings->get_active_colour(i);
        }
    }

    if (!updates) return;
    drawer->draw_objects();
}

}