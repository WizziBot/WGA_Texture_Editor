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
    SIZE
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

private:
vector<uint32_t> colours;
float unit_size;

};

}