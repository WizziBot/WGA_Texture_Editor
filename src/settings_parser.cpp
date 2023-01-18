#include "settings_parser.hpp"
#include <stdio.h>

namespace WinGameAlpha{

App_Settings::App_Settings(wga_err* status){
#define errndie(msg) cerr << "Settings Parser Error: " << msg << endl; \
                     *status = WGA_FAILURE; \
                     return;
#define warn(msg) cerr << "Settings Parser Warning: " << msg << endl; \
                     return;
    // Main parser
    FILE* fd = fopen("settings.wgas","r");
    if (fd == NULL) {
        errndie("Unable to open settings.wgas")
    }
    fseek(fd,0,SEEK_END);
    size_t size = ftell(fd);
    fseek(fd,0,SEEK_SET);

    char* buf = (char*)malloc(sizeof(char)*(size + 1));
    fread(buf, 1, size, fd);
    fclose(fd);
    buf[size] = '\0';

    char lex[64];
    int lexi;

    token_type_t token_type = TAG;
    token_t token = NONE;

    colours.push_back(0x80000000); // Alpha as 0th element
    
    for (int i = 0; i < size; i++){
        switch (buf[i])
        {
        case '\0':
        case '\n':
            if (token_type != BODY){
                errndie("Unexpected newline character")
            }
            if (token == COLOURS){
                lex[lexi] = '\0';
                int colour = (int)std::stoul(lex,nullptr,16);
                colours.push_back(colour);
            } else if (token == SIZE) {
                lex[lexi] = '\0';
                unit_size = (float)std::stoul(lex,nullptr,16);
                cout << "RUS: " << unit_size << endl;
            }
            lexi = 0;
            token_type = TAG;
            token = NONE;
            break;
        case '\r':
        case ' ':
            break;
        case ',':
            if (token == COLOURS){
                lex[lexi] = '\0';
                int colour = (int)std::stoul(lex,nullptr,16);
                colours.push_back(colour);
            } else if (token == SIZE) {
                lex[lexi] = '\0';
                unit_size = (float)std::stoul(lex,nullptr,16);
            } else {
                errndie("Unexpected ',' character")
            }
            lexi = 0;
            break;
        case ':':
            if (token_type == TAG){
                lex[lexi] = '\0';
                if(!strncmp(lex,"colours",63)){
                    token = COLOURS;
                } else if (!strncmp(lex,"size",63)){
                    token = SIZE;
                } else {
                    warn("Unknown tag '" << lex <<"'")
                }
                token_type = BODY;
                lexi = 0;
            }
            break;
        default:
            if (token_type == TAG || token_type == BODY){
                lex[lexi++] = buf[i];
                if (lexi == 64) {
                    errndie("Lexical Buffer Overflow")
                }
            }
            break;
        }
    }
    *status = WGA_SUCCESS;
}

}



