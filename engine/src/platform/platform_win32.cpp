#include "platform.h"

#if SIREN_PLATFORM_WINDOWS

#include "core/logger.h"

#include <windows.h>
#include <windowsx.h>
#include <cstdio>

struct InternalState {
    HINSTANCE h_instance;
    HWND hwnd;
};

// Clock
static double clock_frequency;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t message, WPARAM w_param, LPARAM l_param);

bool platform_init(PlatformState* platform_state, const char* application_name, int x, int y, int width, int height) {
    platform_state->internal_state = malloc(sizeof(InternalState));
    InternalState* state = (InternalState*)platform_state->internal_state;

    state->h_instance = GetModuleHandleA(0);

    LPCSTR WINDOW_CLASS_NAME = "siren_window_class";
    HICON icon = LoadIcon(state->h_instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS; // get double-clicks
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = WINDOW_CLASS_NAME;

    if (!RegisterClassA(&wc)) {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    // Create window
    uint32_t client_x = x;
    uint32_t client_y = y;
    uint32_t client_width = width;
    uint32_t client_height = height;

    uint32_t window_x = client_x;
    uint32_t window_y = client_y;
    uint32_t window_width = client_width;
    uint32_t window_height = client_height;

    uint32_t window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_THICKFRAME;
    uint32_t window_ex_style = WS_EX_APPWINDOW;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    window_x += border_rect.left;
    window_y += border_rect.top;
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(window_ex_style, WINDOW_CLASS_NAME, application_name, window_style, window_x, window_y, window_width, window_height, 0, 0, state->h_instance, 0);

    if (handle == NULL) {
        MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

        SIREN_FATAL("Window creation failed!");
        return false;
    } else {
        state->hwnd = handle;
    }

    bool should_active = 1;
    int show_window_command_flags = should_active ? SW_SHOW : SW_SHOWNOACTIVATE;
    ShowWindow(state->hwnd, show_window_command_flags);

    // Clock setup
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (double)frequency.QuadPart;
    QueryPerformanceCounter(&start_time);

    return true;
}

void platform_quit(PlatformState* platform_state) {
    InternalState* state = (InternalState*)platform_state->internal_state;

    if (state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = NULL;
    }
}

bool platform_pump_messages(PlatformState* platform_state) {
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
 
    }

    return true;
}

/* Memory */

void* platform_allocate(uint64_t size, bool aligned) {
    return malloc(size);
}

void platform_free(void* block, bool aligned) {
    free(block);
}

void* platform_zero_memory(void* block, uint64_t size) {
    return memset(block, 0, size);
}

void* platform_copy_memory(void* destination, const void* source, uint64_t size) {
    return memcpy(destination, source, size);
}

void* platform_set_memory(void* destination, int value, uint64_t size) {
    return memset(destination, value, size);
}

/* Console Write */

void platform_console_write(const char* message, uint8_t color) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static uint8_t levels[6] = { 64, 4, 6, 2, 1, 8 };
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugStringA(message);
    uint64_t length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, number_written, 0);
}

void platform_console_write_error(const char* message, uint8_t color) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    // FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static uint8_t levels[6] = { 64, 4, 6, 2, 1, 8 };
    SetConsoleTextAttribute(console_handle, levels[color]);
    OutputDebugStringA(message);
    uint64_t length = strlen(message);
    LPDWORD number_written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, number_written, 0);
}

/* Clock */

double platform_get_absolute_time() {
    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    return (double)current_time.QuadPart * clock_frequency;
}

void platform_sleep(uint64_t ms) {
    Sleep(ms);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, uint32_t message, WPARAM w_param, LPARAM l_param) {
    switch(message) {
        case WM_ERASEBKGND:
            // Notify the OS that erasing the screen will be handled by the application, which prevents flicker from occuring
            return 1;
        case WM_CLOSE:
            // TODO: fire event for application to quit
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            // Get updated window size
            // RECT r;
            // GetClientRect(hwnd, &r);
            // uint32_t width = r.right - r.left;
            // uint32_t height = r.bottom - r.top;

            // TODO: fire window resize event
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            // bool pressed = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
            // TODO input processing
        } break;
        case WM_MOUSEMOVE: {
            // int x_position = GET_X_LPARAM(l_param);
            // int y_position = GET_Y_LPARAM(l_param);
            // TODO input processing
        } break;
        case WM_MOUSEWHEEL: {
            // int z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            // if (z_delta != 0) {
                // Flatten the input to an OS-independent -1 to 1
                // z_delta = (z_delta < 0) ? -1 : 1;
                // TODO input processing
            // }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            // bool pressed = message == WM_LBUTTONDOWN || message = WM_MBUTTONDOWN || message == WM_RBUTTONDOWN;
            // TODO input processing
        } break;
    }

    // call default window procedure for anything that we're not explicitly handling
    return DefWindowProcA(hwnd, message, w_param, l_param);
}

#endif // SIREN_PLATFORM_WINDOWS