
#include "core.hpp"
#include "common.hpp"
#include "utils.hpp"
#include "renderer.hpp"
#include "app.hpp"

/* DEFINES */
#define W_WIDTH 800
#define W_HEIGHT 450
#define TICK_DELAY 1
#define C_ONMSG "WGA Texture Editor: Started"

namespace WinGameAlpha {

Render_State render_state;

bool running = false;
bool resizing = false;
bool resized = false;
bool updates = false;

BOOL WindowResize(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int width = LOWORD(lParam);
    int height = HIWORD(lParam);

    if (width < (height * W_WIDTH) / W_HEIGHT) {
        // Calculate correct aspect ratio size
        width = (height * W_WIDTH) / W_HEIGHT;
        SetWindowPos(hWnd, NULL, 0, 0,
                     width, height,
                     SWP_NOMOVE | SWP_NOZORDER);
    }
    
    return true;
}

BOOL WindowResizing(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PRECT rectp = (PRECT)lParam;

    RECT rect;
    GetClientRect(hWnd, &rect);

    int width  = W_WIDTH ;
    int height = W_HEIGHT;

    // Minimum size
    if (rectp->right - rectp->left < width)
	rectp->right = rectp->left + width;

    if (rectp->bottom - rectp->top < height)
	rectp->bottom = rectp->top + height;

    return true;
}

LRESULT window_callback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam){
    LRESULT result = 0;
    switch(Msg){
        case WM_CLOSE:
        case WM_DESTROY: {
            running = false;
        } break;
        case WM_SIZING: {
            if (!WindowResizing(hWnd,wParam,lParam)){
                running = false;
                break;
            }
            resizing = true;
        } break;
        case WM_SIZE: {
            if (!WindowResize(hWnd,wParam,lParam)) {
                running = false;
                break;
            }
            updates = true;
            if (resizing) resized = true;
            resizing = false;
            RECT rect;
            GetClientRect(hWnd,&rect);
            render_state.width = rect.right - rect.left;
            render_state.height = rect.bottom - rect.top;
#ifndef USING_OPENCL
            int buffer_size = render_state.width*render_state.height*sizeof(uint32_t); //3 bytes for RGB and 1 byte padding
            if (render_state.memory) VirtualFree(render_state.memory,0,MEM_RELEASE);
            render_state.memory = VirtualAlloc(0, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (render_state.memory == NULL){
                std::cerr << "Memory assignment failure: Render state" << std::endl;
            }
#endif
            render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
            render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
        } break;

        default: {
            result = DefWindowProc(hWnd,Msg,wParam,lParam);
        }
    }
    return result;
}
}

using namespace WinGameAlpha;

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
    
    std::cout << C_ONMSG << std::endl;

    // Init bitmap header
    render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
    render_state.bitmap_info.bmiHeader.biPlanes = 1;
    render_state.bitmap_info.bmiHeader.biBitCount = 32;
    render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

    // Create Window Class
    WNDCLASS window_class = {};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpszClassName = (LPCTSTR)"Game Window Class";
    window_class.lpfnWndProc = (WNDPROC)window_callback;

    // Register Class
    RegisterClass(&window_class);
    // Create Window
    HWND window = CreateWindowA((LPCSTR)window_class.lpszClassName, (LPCSTR)"WinGameAlpha", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, W_WIDTH, W_HEIGHT, 0, 0, hInstance,0);
    HDC hdc = GetDC(window);
    // Frist time render
    running = true;
    render_init();
    Input input;

    float delta_time = 0.016666f; //Initial 60 fps assumption, immediately reassigned after first tick
    LARGE_INTEGER frame_begin_time, frame_end_time;
    QueryPerformanceCounter(&frame_begin_time);
    float performance_frequency;
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        performance_frequency = (float)perf.QuadPart;
    }
    HINSTANCE hInst = GetModuleHandle(NULL);
    HCURSOR point_cursor = LoadCursorA(NULL,(LPCSTR)IDC_ARROW);
    // TRACKMOUSEEVENT tme = {
    //     .dwFlags = TME_HOVER,
    //     .hwndTrack = window,
    //     .dwHoverTime = 10
    // };
    // tme.cbSize = sizeof(tme);
    // TrackMouseEvent((LPTRACKMOUSEEVENT)&tme);
    bool in_wnd = true;
    while (running){
        MSG message;
        for (int i = 0; i < BUTTON_COUNT; i++){
            input.buttons[i].changed = false;
        }
        while (PeekMessage(&message,window,0,0,PM_REMOVE)) {
            switch(message.message) {
                case WM_KEYUP: 
                case WM_KEYDOWN: {
                    uint32_t vk_code = (uint32_t)message.wParam;
                    bool is_down = ((message.lParam & (1<<31)) == 0);
                    int key;
                    if (vk_code >= 0x30 && vk_code <= 0x39) {
                        key = vk_code % 0x30;
                        for (int i = 0; i < BUTTON_COUNT; i++){
                            input.buttons[i].down = false;
                        }
                        input.buttons[key].down = true;
                    }
                    else if (vk_code == VK_S && (GetKeyState(VK_LCONTROL) >> 15)) {
                        key = BUTTON_CTRL_S;
                        input.buttons[key].changed = (is_down != input.buttons[key].down);
                        input.buttons[key].down = is_down;
                    }
                    else break;
                } break;
                case WM_LBUTTONUP:
                case WM_LBUTTONDOWN:{
                    input.mouse_state.changed = true;
                    input.mouse_state.down = (MK_LBUTTON & message.wParam);
                }break;
                case WM_MOUSEMOVE:{
                    SetCursor(point_cursor);
                    input.mouse_state.x_pos = LOWORD(message.lParam);
                    input.mouse_state.y_pos = HIWORD(message.lParam);
                }break;
                case WM_MOUSELEAVE:{
                    input.mouse_state.changed = true;
                    input.mouse_state.down = false;
                };
                default: {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
            }
        }

        Sleep(TICK_DELAY);

        if (resized) {
            render_update();
            resized = false;
        }
        render_tick(input,delta_time);
        // Overwrite screen buffer
        if (updates){
            if (render_state.memory){
                StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
            }
            updates = false;
        }

        // SPF calculation
        QueryPerformanceCounter(&frame_end_time);
        delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart)/performance_frequency;
        frame_begin_time = frame_end_time;
    }
    app_cleanup();
    return 0;
}