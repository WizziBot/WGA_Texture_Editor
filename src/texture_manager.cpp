#include "texture_manager.hpp"
#include "renderer.hpp"

namespace WinGameAlpha {

void Texture_Manager::create_render_object(shared_ptr<Render_Matrix> render_matrix, int render_layer){
    render_objects.push_back(Render_Object(m_drawer,render_matrix,render_layer,false));
}

shared_ptr<Render_Matrix> Texture_Manager::create_render_matrix(float x_offset, float y_offset, float width, float height, uint32_t* matrix, float unit_size_x, float unit_size_y){
    render_matrices.push_back(make_shared<Render_Matrix>(x_offset, y_offset, width, height, matrix, unit_size_x, unit_size_y));
    return render_matrices.back();
}

wga_err Texture_Manager::load_character_textures(){
    wga_err err;

    // Load character textures into char lib
    int width, height;
    float unit_size;
    uint32_t* matrix;
    // Numbers
    char num = '0';
    string curr = "./textures/text/";
    for (int i=0; i<10;i++){
        curr += (num+i);
        curr += ".wgat";
        err = load_texture(&matrix,&width,&height,&unit_size,curr);
        TEXEX("Could not load texture " << curr,err)
        curr = "./textures/text/";
        if (i==0) m_char_width = width;
        shared_ptr<Render_Matrix> temp = create_render_matrix(0,0,width,height,matrix,unit_size,unit_size);
        m_char_lib.character_list.push_back(temp);
    }

    return WGA_SUCCESS;
}

wga_err Texture_Manager::register_all_objects(){
    wga_err err;
    vector<Render_Object>::iterator iter;
    for (iter = render_objects.begin(); iter != render_objects.end(); iter++){
        shared_ptr<Render_Object> new_obj((Render_Object*)iter.base());
        err = m_drawer->register_render_object(new_obj);
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