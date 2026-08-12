#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>
#include "maths.h"

// Key Codes & Mouse Buttons
#define KEY_NULL            0
#define KEY_APOSTROPHE      39
#define KEY_COMMA           44
#define KEY_MINUS           45
#define KEY_PERIOD          46
#define KEY_SLASH           47
#define KEY_ZERO            48
#define KEY_ONE             49
#define KEY_TWO             50
#define KEY_THREE           51
#define KEY_FOUR            52
#define KEY_FIVE            53
#define KEY_SIX             54
#define KEY_SEVEN           55
#define KEY_EIGHT           56
#define KEY_NINE            57
#define KEY_SEMICOLON       59
#define KEY_EQUAL           61
#define KEY_A               65
#define KEY_B               66
#define KEY_C               67
#define KEY_D               68
#define KEY_E               69
#define KEY_F               70
#define KEY_G               71
#define KEY_H               72
#define KEY_I               73
#define KEY_J               74
#define KEY_K               75
#define KEY_L               76
#define KEY_M               77
#define KEY_N               78
#define KEY_O               79
#define KEY_P               80
#define KEY_Q               81
#define KEY_R               82
#define KEY_S               83
#define KEY_T               84
#define KEY_U               85
#define KEY_V               86
#define KEY_W               87
#define KEY_X               88
#define KEY_Y               89
#define KEY_Z               90
#define KEY_SPACE           32
#define KEY_ESCAPE          256
#define KEY_ENTER           257
#define KEY_TAB             258
#define KEY_BACKSPACE       259
#define KEY_RIGHT           262
#define KEY_LEFT            263
#define KEY_DOWN            264
#define KEY_UP              265
#define KEY_LEFT_SHIFT      340
#define KEY_LEFT_CONTROL    341
#define KEY_LEFT_ALT        342

#define MOUSE_BUTTON_LEFT   0
#define MOUSE_BUTTON_RIGHT  1
#define MOUSE_BUTTON_MIDDLE 2

// Core Window & Input API Functions
void init_window(int width, int height, const char *title);
bool window_should_close(void);
void close_window(void);

void set_target_fps(int fps);
float get_frame_time(void);
double get_time(void);
int get_fps(void);

int get_screen_width(void);
int get_screen_height(void);

bool is_key_down(int key);
bool is_key_pressed(int key);
bool is_key_released(int key);

bool is_mouse_button_down(int button);
bool is_mouse_button_pressed(int button);
bool is_mouse_button_released(int button);

Vector2 get_mouse_position(void);
Vector2 get_mouse_delta(void);
float get_mouse_wheel_move(void);

#endif // WINDOW_H
