#pragma once
#include "core.hpp"

namespace WinGameAlpha{

enum token_t {
    NONE,
    TAG,
    BODY
};

class App_Settings {
public:
/* Gets settings.wgas file and parses it, initializing the settings of the app.
    @param status the location for the return error
*/
App_Settings(wga_err* status);

/* Set the active colour from colour array when changed by the user*/
void set_active_colour(int index){
    active_colour = colours.at(index);
}

uint32_t active_colour;
private:
vector<uint32_t> colours;
float unit_size;

};

}