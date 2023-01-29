#pragma once
#include "core.hpp"

namespace WinGameAlpha{

enum token_type_t {
    TAG,
    BODY
};
enum token_t {
    NONE,
    COLOURS,
    SIZE,
    DIMS
};

class App_Settings {
public:
/* Gets settings.wgas file and parses it, initializing the settings of the app.
    @param status the location for the return error
*/
App_Settings(wga_err* status);

/* Set the active colour from colour array when changed by the user*/
uint32_t get_active_colour(int index){
    return colours.at(index);
}
float get_unit_size(){
    return unit_size;
}
int get_colours_size(){
    return colours.size();
}
int get_swidth(){
    return s_width;
}
int get_sheight(){
    return s_height;
}
bool using_sdims(){
    return strict_dims;
}

private:
vector<uint32_t> colours;
float unit_size;
bool strict_dims;
int s_width;
int s_height;

};

}