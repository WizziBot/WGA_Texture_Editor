#include "texture_manager.hpp"

namespace WinGameAlpha {

wga_err Texture_Manager::create_render_object(Render_Matrix* render_matrix, int render_layer){
    wga_err err;
    render_objects.push_back(Render_Object(m_drawer,render_matrix,render_layer));
    err = m_drawer->register_render_object(&render_objects.back());
    return err;
}
Render_Matrix* Texture_Manager::create_render_matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y){
    render_matrices.push_back(Render_Matrix(x_offset, y_offset, width, height, matrix, unit_size_x, unit_size_y));
    return &render_matrices.back();
}

}