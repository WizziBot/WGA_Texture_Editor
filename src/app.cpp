
#include "app.hpp"
#include "render_objects.hpp"
#include "texture_manager.hpp"
#include "renderer.hpp"
#include "settings_parser.hpp"
#include <filesystem>

namespace fs = std::filesystem;

#define CANVAS_WIDTH 185.f
#define CANVAS_HEIGHT 95.f
#define BACKGROUND_COLOUR 0x252526
#define ARENA_COLOUR 0x1e1e1e
#define CANVAS_RENDER_LAYER 0
#define INDICATOR_RENDER_LAYER 1
#define TEXT_RENDER_LAYER 2
#define TEXT_SIZE 0.5

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
int colours_size;
uint32_t active_colour = 0;
shared_ptr<Drawer> drawer;
shared_ptr<Texture_Manager> texture_manager;
shared_ptr<Render_Matrix> canvas;
shared_ptr<App_Settings> settings;
shared_ptr<Text_Object> drw_width;
shared_ptr<Text_Object> drw_height;
bool first_stroke = true;
bool using_sdims = false;
string load_texture_name;

uint32_t colour_indicator[] {
    0x000000
};
uint32_t background_matrix[] {
    ARENA_COLOUR
};

// Functions

inline void memset32(void *m, uint32_t val, size_t count)
{
    uint32_t *buf = (uint32_t*)m;
    while(count--) *buf++ = val;
}

void update_drw_dims(int width, int height){
    string w = std::to_string(width);
    string h = std::to_string(height);
    drw_width->change_text(w);
    drw_height->change_text(h);
}

void process_mouse_down(int mouse_x, int mouse_y){
    // Convert to abs coords
    float factor = (float)render_state.height/100.f;
    int cv_unit_size = floor(canvas_unit_size*factor);
    int cv_width = cv_unit_size*canvas_width;
    int cv_height = cv_unit_size*canvas_height;
    int canvas_horz_border = floor((float)(render_state.width-cv_width)/2.f);
    int canvas_vert_border = floor((float)(render_state.height-cv_height)/2.f);
    if (within_bounds(canvas_horz_border,mouse_x,cv_width+canvas_horz_border) && within_bounds(canvas_vert_border,mouse_y,cv_height+canvas_vert_border)){
        // Find canvas matrix square
        int matrix_x = (mouse_x-canvas_horz_border)/cv_unit_size;
        int matrix_y = (mouse_y-canvas_vert_border)/cv_unit_size;
        int matrix_index = matrix_x + canvas_width*matrix_y;
        
        canvas_matrix[matrix_index] = active_colour;
        updates = true;

        if (using_sdims) return;
        bool changed = false;
        // Adjust submatrix boundary
        if ((cv_higher_x-cv_lower_x == 0 || cv_higher_y-cv_lower_y == 0) && first_stroke){
            cv_higher_x = cv_lower_x = matrix_x;
            cv_higher_y = cv_lower_y = matrix_y;
            first_stroke = false;
        }
        if (active_colour != ALPHA_BIT){
            if (matrix_x < cv_lower_x) cv_lower_x = matrix_x, changed = true;
            if (matrix_y < cv_lower_y) cv_lower_y = matrix_y, changed = true;
            if (matrix_x > cv_higher_x) cv_higher_x = matrix_x, changed = true;
            if (matrix_y > cv_higher_y) cv_higher_y = matrix_y, changed = true;
        } else {
            bool can_reduce = true;
            // Calculate whether to decrement submatrix boundary
            // Check all squares within bound on the edge to see if any of them are non-alpha
            int mx = matrix_x;
            int my = matrix_y;
            // Sides
            for (int i=cv_lower_y; i <=cv_higher_y; i++){
                if (mx == cv_lower_x || mx == cv_higher_x){
                    int cv_index = mx + cv_lower_y*canvas_width;
                    for (int y = cv_lower_y; y <= cv_higher_y; y++){
                        if (canvas_matrix[cv_index] != ALPHA_BIT){
                            can_reduce = false;
                            break;
                        }
                        cv_index += canvas_width;
                    }
                    if (can_reduce){
                        if (mx == cv_lower_x) {
                            cv_lower_x++;
                            mx++;
                        }
                        else {
                            cv_higher_x--;
                            mx--;
                        }
                        changed = true;
                    } else {
                        break;
                    }
                }
            }
            
            can_reduce = true;
            // Top & Bottom
            for (int i=cv_lower_x; i <=cv_higher_x; i++){
                if (my == cv_lower_y || my == cv_higher_y) {
                    int cv_index = my*canvas_width + cv_lower_x;
                    for (int x = cv_lower_x; x <= cv_higher_x; x++){
                        if (canvas_matrix[cv_index] != ALPHA_BIT){
                            can_reduce = false;
                            break;
                        }
                        cv_index++;
                    }
                    if (can_reduce){
                        if (my == cv_lower_y) {
                            cv_lower_y++;
                            my++;
                        }
                        else {
                            cv_higher_y--;
                            my--;
                        }
                        changed = true;
                    } else {
                        break;
                    }
                }
            }
            if (cv_lower_x > cv_higher_x){
                cv_higher_x = cv_lower_x;
                first_stroke = true;
            } else if (cv_lower_y > cv_higher_y){
                cv_higher_y = cv_lower_y;
                first_stroke = true;
            }
        }
        if (changed) update_drw_dims(cv_higher_x-cv_lower_x+1,cv_higher_y-cv_lower_y+1);
    }
}

wga_err get_load_texture_name() {
    for (auto &p : fs::recursive_directory_iterator(".")) {
        if (p.path().extension() == ".wgat" && p.path().string().find("textures\\") == string::npos){
            load_texture_name = p.path().generic_string();
            return WGA_SUCCESS;
        }
    }
    return WGA_FAILURE;
}

void save_canvas(){
    uint32_t* submatrix = texture_manager->crop_matrix(canvas_matrix,canvas_width,cv_lower_x,cv_lower_y,cv_higher_x,cv_higher_y);
    if (submatrix != NULL){
        cout << "[Matrix] Width: " << cv_higher_x-cv_lower_x+1 << " Height: " << cv_higher_y-cv_lower_y+1 << " Size: " << canvas_unit_size << endl;
        cout << "[Total] Width: " << (cv_higher_x-cv_lower_x+1)*canvas_unit_size << " Height: " << (cv_higher_y-cv_lower_y+1)*canvas_unit_size << endl;
        wga_err err = texture_manager->save_texture(submatrix,(cv_higher_x-cv_lower_x+1),(cv_higher_y-cv_lower_y+1),canvas_unit_size,load_texture_name);
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
    cv_higher_x = x1-1;
    cv_lower_y = y0;
    cv_higher_y = y1-1;
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

    // Load texture
    int width=0,height=0;
    float ld_unit_size = 0;
    err = get_load_texture_name();
    if (err == WGA_FAILURE) load_texture_name = "texture.wgat";
    else err = texture_manager->load_texture(&loaded_texture,&width,&height,&ld_unit_size,load_texture_name);

    // Setup canvas
    colours_size = settings->get_colours_size();
    if ((canvas_unit_size = settings->get_unit_size()) == 0) canvas_unit_size = ld_unit_size;
    if (canvas_unit_size <= 0) {
        WGACHECKERRNO("Unable to resolve unit size of canvas",WGA_FAILURE);
        return;
    }
    canvas_width = floor(CANVAS_WIDTH/canvas_unit_size);
    canvas_height = floor(CANVAS_HEIGHT/canvas_unit_size);
    cv_higher_x = cv_lower_x = canvas_width/2;
    cv_higher_y = cv_lower_y = canvas_height/2;
    canvas_matrix = (uint32_t*)VirtualAlloc(0,canvas_height*canvas_width*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    memset32(canvas_matrix,AB,canvas_height*canvas_width);

    if (err == WGA_SUCCESS){
        load_onto_canvas(loaded_texture,width,height);
        cout << "Loaded Texture: " << load_texture_name << endl;
    } else cout << "No texture loaded" << endl;

    // Strict dims
    using_sdims = settings->using_sdims();
    if (using_sdims){
        int x0 = floor((float)canvas_width/2.f - (float)settings->get_swidth()/2.f);
        int y0 = floor((float)canvas_height/2.f - (float)settings->get_sheight()/2.f);
        int x1 = x0 + settings->get_swidth();
        int y1 = y0 + settings->get_sheight();
        cv_lower_x = x0;
        cv_higher_x = x1-1;
        cv_lower_y = y0;
        cv_higher_y = y1-1;
        // Left
        int cv_index = cv_lower_x-1 + cv_lower_y*canvas_width;
        for (int y = cv_lower_y; y <= cv_higher_y; y++){
            canvas_matrix[cv_index] = 0xcc5454;
            cv_index += canvas_width;
        }
        // Right
        cv_index = cv_higher_x+1 + cv_lower_y*canvas_width;
        for (int y = cv_lower_y; y <= cv_higher_y; y++){
            canvas_matrix[cv_index] = 0xcc5454;
            cv_index += canvas_width;
        }
        // Bottom
        cv_index = (cv_lower_y-1)*canvas_width + cv_lower_x-1;
        for (int x = cv_lower_x; x <= cv_higher_x+2; x++){
            canvas_matrix[cv_index] = 0xcc5454;
            cv_index++;
        }
        // Top
        cv_index = (cv_higher_y+1)*canvas_width + cv_lower_x-1;
        for (int x = cv_lower_x; x <= cv_higher_x+2; x++){
            canvas_matrix[cv_index] = 0xcc5454;
            cv_index++;
        }
    }

    float factor = (float)render_state.height/100.f;

    canvas = texture_manager->create_render_matrix(0,0,(float)canvas_width,(float)canvas_height,canvas_matrix,canvas_unit_size,canvas_unit_size);
    texture_manager->create_render_object(canvas,CANVAS_RENDER_LAYER);
    shared_ptr<Render_Matrix> c_indicator = texture_manager->create_render_matrix(-CANVAS_WIDTH/2 - 3,CANVAS_HEIGHT/2 - 2,1,1,colour_indicator,4,4);
    texture_manager->create_render_object(c_indicator,INDICATOR_RENDER_LAYER);
    WGAERRCHECK(texture_manager->register_all_objects());
    drawer->set_background_colour(BACKGROUND_COLOUR);
    texture_manager->load_character_textures();
    drw_width = make_shared<Text_Object>(drawer,texture_manager,"0",77,42,TEXT_SIZE,texture_manager->get_char_width(),TEXT_RENDER_LAYER);
    drw_height = make_shared<Text_Object>(drawer,texture_manager,"0",87,42,TEXT_SIZE,texture_manager->get_char_width(),TEXT_RENDER_LAYER);
    update_drw_dims(cv_higher_x-cv_lower_x+1,cv_higher_y-cv_lower_y+1);
}

void render_update(){
#ifdef USING_OPENCL
    WGAERRCHECK(drawer->cl_resize());
#endif
}

void render_tick(Input& input, float dt){
    // Add toggle for grid
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
            colour_indicator[0] = active_colour;
            updates = true;
            input.buttons[i].down = false;
        }
    }

    if (!updates) return;
    drawer->draw_objects();
}

}