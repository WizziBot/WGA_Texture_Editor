#include "settings_parser.hpp"
#include <stdio.h>

namespace WinGameAlpha{

App_Settings::App_Settings(wga_err* status){
#define errndie(msg) cerr << "Settings Parser Error: " << msg << endl; \
                     *status = WGA_FAILURE; \
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

    token_t token = TAG;

    for (int i = 0; i < size; i++){
        switch (buf[i])
        {
        case '\r':
            i++;
            break;
        case '\n':
            if (token != BODY){
                errndie("Unexpected newline character")
            }

            break;
        case ' ':
            i++;
            break;
        case ',':
            break;
        case ':':
            break;
        default:
            if (token == TAG || token == BODY){
                lex[lexi++] = buf[i];
                if (lexi == 64) {
                    errndie("Lexical Buffer Overflow")
                }
            }
            break;
        }
    }
}

}



