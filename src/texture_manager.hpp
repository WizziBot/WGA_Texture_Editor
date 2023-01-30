#pragma once
#include "core.hpp"
#include "render_objects.hpp"

namespace WinGameAlpha {

#define TEXEX(msg,err_no) if (err_no != WGA_SUCCESS) { cerr << "Texture Manager Error: " \
                      << msg << endl;\
                      return WGA_FAILURE;}


class Texture_Manager {
public:
/* Manages and stores render objects and other textures
    @param drawer pointer to drawer object
*/
Texture_Manager(shared_ptr<Drawer> drawer){
    m_drawer = drawer;
}
~Texture_Manager();
/* (PHASE 1) Render object which contains matrices to be rendered on each draw() call if registered
    @param render_matrix_index the render matrix's index number returned by create_render_matrix()
    @param render_layer the id of the render layer of the object where the render objects within the layer will be rendered together,
    the render layers must be declared contiguously i.e. layer 0 must exist before layer 1
    @return WGA_SUCCESS on succes and WGA_FAILURE on failure
*/
wga_err create_render_object(int render_matrix_index, int render_layer);
/* Must be ran at the end of (PHASE 0) 
    Loads character textures. Initialises the Character_Library and fills it with the Render_Matrix objects retrieved by loading the textures
*/
wga_err load_character_textures();
/* (PHASE 0) Render matrix is a matrix type texture
    @param x_offset the x_offset from the related object's coordinates
    @param y_offset the y_offset from the related object's coordinates
    @param width the number of elements in width of the matrix
    @param height the number of elements in height of the matrix
    @param matrix a pointer to an array of colours for each unit element
    @param unit_size_x the width of each square unit in relative size
    @param unit_size_y the height of each square unit in relative size
    @return The index of the render matrix, returns -1 on failure
*/
int create_render_matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y);
/* Increments the registration phase, part of a safety mechanism to ensure render objects are created in the correct order.
    - Render Matrices (phase 0)
    - Render Objects (phase 1)
    - Register render objects with register_all_objects() (phase 3)
*/
void next_registration_phase(){
    registration_phase++;
}
/* Register all objects, must be called at the end of (PHASE 1)  */
wga_err register_all_objects();
/* Save a render matrix texture as a .wgat file
    @param matrix pointer to the raw matrix of the render matrix to be copied from
    @param widht the width of the matrix
    @param height the height of the matrix
    @param unit_size the unit size of the matrix
    @param file_name the name of the file
*/
static wga_err save_texture(uint32_t* matrix, int width, int height, float unit_size, string file_name);
/* Read a render matrix texture from a .wgat file
    @param matrix_dst double pointer to the location where the render matrix will be stored
    @param width a pointer to where the width will be stored
    @param height a pointer to where the height will be stored
    @param unit_size a pointer to where the unit size will be stored
    @param file_name the name of the file
*/
wga_err load_texture(uint32_t** matrix_dst, int* width, int* height, float* unit_size, string file_name);
/* Crop a submatrix out of a larger matrix
    @param matrix a pointer to the main matrix
    @param matrix_width the width of tha main matrix
    @param x0_y0_x1_y1 Coordinates of corners within the large matrix being the corners of the submatrix
    @returns a pointer to the submatrix or NULL on failure
*/
static uint32_t* crop_matrix(uint32_t* matrix, int matrix_width, int x0, int y0, int x1, int y1);

Character_Library* get_char_lib_ptr(){
    return &m_char_lib;
}

int get_char_width(){
    return m_char_width;
}

private:
shared_ptr<Drawer> m_drawer;
Character_Library m_char_lib;
int m_char_width=0;
int registration_phase = 0;
vector<Render_Object> render_objects;
vector<Render_Matrix> render_matrices;
};

}