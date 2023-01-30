#include "texture_manager.hpp"
#include "renderer.hpp"

namespace WinGameAlpha {

Texture_Manager::~Texture_Manager(){
    for (auto matrix : render_matrices){
        matrix.free_matrix();
    }
}

wga_err Texture_Manager::create_render_object(int render_matrix_index, int render_layer){
    if (registration_phase != 1) return WGA_FAILURE;
    if (render_matrix_index < 0 || render_matrix_index >= render_matrices.size()) return WGA_FAILURE;
    Render_Matrix* render_matrix = &render_matrices.at(render_matrix_index);
    render_objects.push_back(Render_Object(m_drawer,render_matrix,render_layer,false));
    return WGA_SUCCESS;
}

int Texture_Manager::create_render_matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y){
    if (registration_phase != 0) return WGA_FAILURE;
    render_matrices.push_back(Render_Matrix(x_offset, y_offset, width, height, matrix, unit_size_x, unit_size_y));
    return render_matrices.size()-1;
}

wga_err Texture_Manager::load_character_textures(){
    if (registration_phase != 0) return WGA_FAILURE;
    wga_err err;

    // Load character textures into char lib
    int width, height;
    float unit_size;
    uint32_t* matrix;
    // Numbers
    char num = '0';
    string curr = "./textures/text/";
    int matrix_idx[10] = {0};
    int i;
    for (i=0; i<10;i++){
        curr += (num+i);
        curr += ".wgat";
        err = load_texture(&matrix,&width,&height,&unit_size,curr);
        TEXEX("Could not load texture " << curr,err)
        curr = "./textures/text/";
        if (i==0) m_char_width = width;
        matrix_idx[i] = create_render_matrix(0,0,width,height,matrix,unit_size,unit_size);
    }
    Render_Matrix* render_matrix;
    for (i=0;i<10;i++){
        render_matrix = &render_matrices.at(matrix_idx[i]);
        m_char_lib.character_list.push_back(render_matrix);
    }

    return WGA_SUCCESS;
}

wga_err Texture_Manager::register_all_objects(){
    if (registration_phase != 1) return WGA_FAILURE;
    registration_phase++;

    wga_err err;
    vector<Render_Object>::iterator iter;
    for (iter = render_objects.begin(); iter != render_objects.end(); iter++){
        err = m_drawer->register_render_object(iter.base());
        if (err != WGA_SUCCESS) return WGA_FAILURE;
    }
    return WGA_SUCCESS;
}

wga_err Texture_Manager::save_texture(uint32_t* matrix, int width, int height, float unit_size, string file_name){
    wga_err err;
    FILE* fd = fopen(file_name.c_str(),"w");
    if (fd == NULL) {return WGA_FAILURE;}
    int written = fwrite(&width,sizeof(int),1,fd);
    written += fwrite(&height,sizeof(int),1,fd);
    written += fwrite(&unit_size,sizeof(float),1,fd);
    written += fwrite(matrix,sizeof(uint32_t),width*height,fd);
    if (written == width*height + 3) err = WGA_SUCCESS;
    else err = WGA_FAILURE;
    fclose(fd);
    return err;
}

wga_err Texture_Manager::load_texture(uint32_t** matrix_dst, int* width, int* height, float* unit_size, string file_name){
    wga_err err;
    FILE* fd = fopen(file_name.c_str(),"r");
    if (fd == NULL) {return WGA_FAILURE;}
    int _width,_height;
    float _unit_size;
    int read = fread(&_width,sizeof(int),1,fd);
    read += fread(&_height,sizeof(int),1,fd);
    read += fread(&_unit_size,sizeof(float),1,fd); 
    uint32_t* matrix = (uint32_t*)VirtualAlloc(0,_width*_height*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    read += fread(matrix,sizeof(uint32_t),_width*_height,fd);
    if (read == _width*_height + 3) err = WGA_SUCCESS;
    else err = WGA_FAILURE;
    fclose(fd);
    *width = _width;
    *height = _height;
    *unit_size = _unit_size;
    *matrix_dst = matrix;
    return err;
}

uint32_t* Texture_Manager::crop_matrix(uint32_t* matrix, int matrix_width, int x0, int y0, int x1, int y1){
    int width = x1-x0+1;
    int height = y1-y0+1;
    if (width == 0 || height == 0) return NULL;
    uint32_t* submatrix = (uint32_t*)VirtualAlloc(0,width*height*sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (submatrix==NULL) return NULL;
    uint32_t* mp = matrix;
    uint32_t* smp = submatrix;
    for (int y=y0; y <= y1; y++){
        for (int x=x0; x <= x1; x++){
            *smp = matrix[y*matrix_width + x];
            smp++;
        }
    }
    return submatrix;
}

}