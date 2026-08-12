#include "window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEYS 512
#define MAX_MOUSE_BUTTONS 8

// Internal Window & Input State
typedef struct WindowContext {
    GLFWwindow *handle;
    int width;
    int height;
    const char *title;
    bool should_close;

    // Timing
    double current_frame_time;
    double last_frame_time;
    float frame_time;
    int target_fps;
    double target_frame_duration;
    int current_fps;
    int frame_counter;
    double last_fps_update_time;

    // Input state
    bool keys_down[MAX_KEYS];
    bool keys_pressed[MAX_KEYS];
    bool keys_released[MAX_KEYS];

    bool mouse_buttons_down[MAX_MOUSE_BUTTONS];
    bool mouse_buttons_pressed[MAX_MOUSE_BUTTONS];
    bool mouse_buttons_released[MAX_MOUSE_BUTTONS];

    Vector2 mouse_pos;
    Vector2 last_mouse_pos;
    Vector2 mouse_delta;
    float mouse_wheel;
} WindowContext;

static WindowContext g_win_ctx = { 0 };

// Forward declaration of internal renderer frame end hook
extern void internal_renderer_begin_frame(int width, int height);
extern void internal_renderer_end_frame(void);

// ----------------------------------------------------------------------------------
// GLFW Callbacks
// ----------------------------------------------------------------------------------

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    (void)window; (void)scancode; (void)mods;
    if (key < 0 || key >= MAX_KEYS) return;

    if (action == GLFW_PRESS) {
        g_win_ctx.keys_down[key] = true;
        g_win_ctx.keys_pressed[key] = true;
    } else if (action == GLFW_RELEASE) {
        g_win_ctx.keys_down[key] = false;
        g_win_ctx.keys_released[key] = true;
    }
}

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    (void)window; (void)mods;
    if (button < 0 || button >= MAX_MOUSE_BUTTONS) return;

    if (action == GLFW_PRESS) {
        g_win_ctx.mouse_buttons_down[button] = true;
        g_win_ctx.mouse_buttons_pressed[button] = true;
    } else if (action == GLFW_RELEASE) {
        g_win_ctx.mouse_buttons_down[button] = false;
        g_win_ctx.mouse_buttons_released[button] = true;
    }
}

static void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos) {
    (void)window;
    g_win_ctx.mouse_pos = (Vector2){ (float)xpos, (float)ypos };
}

static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    (void)window; (void)xoffset;
    g_win_ctx.mouse_wheel += (float)yoffset;
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    (void)window;
    g_win_ctx.width = width;
    g_win_ctx.height = height;
    glViewport(0, 0, width, height);
}

/*
 * Initialize glfw window.
 */
void init_window(int width, int height, const char *title) {
    g_win_ctx.width = width;
    g_win_ctx.height = height;
    g_win_ctx.title = title;
    g_win_ctx.target_fps = 0;
    g_win_ctx.target_frame_duration = 0.0;

    if (!glfwInit()) {
        fprintf(stderr, "[ERROR] Failed to initialize GLFW\n");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    g_win_ctx.handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!g_win_ctx.handle) {
        fprintf(stderr, "[ERROR] Failed to create GLFW Window\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(g_win_ctx.handle);
    glfwSwapInterval(1); // Enable V-Sync by default

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "[ERROR] Failed to initialize GLAD OpenGL loader\n");
        exit(EXIT_FAILURE);
    }

    // Set GLFW Callbacks
    glfwSetKeyCallback(g_win_ctx.handle, key_callback);
    glfwSetMouseButtonCallback(g_win_ctx.handle, mouse_button_callback);
    glfwSetCursorPosCallback(g_win_ctx.handle, cursor_pos_callback);
    glfwSetScrollCallback(g_win_ctx.handle, scroll_callback);
    glfwSetFramebufferSizeCallback(g_win_ctx.handle, framebuffer_size_callback);

    double xpos, ypos;
    glfwGetCursorPos(g_win_ctx.handle, &xpos, &ypos);
    g_win_ctx.mouse_pos = (Vector2){ (float)xpos, (float)ypos };
    g_win_ctx.last_mouse_pos = g_win_ctx.mouse_pos;

    g_win_ctx.last_frame_time = glfwGetTime();
    g_win_ctx.last_fps_update_time = g_win_ctx.last_frame_time;
    g_win_ctx.current_fps = 60;

    internal_renderer_begin_frame(width, height);
}

bool window_should_close(void) {
    if (g_win_ctx.handle == NULL) return true;
    return glfwWindowShouldClose(g_win_ctx.handle) || g_win_ctx.should_close;
}

void close_window(void) {
    if (g_win_ctx.handle) {
        glfwDestroyWindow(g_win_ctx.handle);
        g_win_ctx.handle = NULL;
    }
    glfwTerminate();
}

void set_target_fps(int fps) {
    g_win_ctx.target_fps = fps;
    if (fps > 0) {
        g_win_ctx.target_frame_duration = 1.0 / (double)fps;
        glfwSwapInterval(0); // Disable V-Sync to use target FPS sleep
    } else {
        g_win_ctx.target_frame_duration = 0.0;
        glfwSwapInterval(1);
    }
}

float get_frame_time(void) {
    return g_win_ctx.frame_time;
}

double get_time(void) {
    return glfwGetTime();
}

int get_fps(void) {
    return g_win_ctx.current_fps;
}

int get_screen_width(void) {
    return g_win_ctx.width;
}

int get_screen_height(void) {
    return g_win_ctx.height;
}

// ----------------------------------------------------------------------------------
// Internal Frame Loop Hooks
// ----------------------------------------------------------------------------------

void internal_window_poll_events(void) {
    // Reset single-frame key & mouse press/release flags
    memset(g_win_ctx.keys_pressed, 0, sizeof(g_win_ctx.keys_pressed));
    memset(g_win_ctx.keys_released, 0, sizeof(g_win_ctx.keys_released));
    memset(g_win_ctx.mouse_buttons_pressed, 0, sizeof(g_win_ctx.mouse_buttons_pressed));
    memset(g_win_ctx.mouse_buttons_released, 0, sizeof(g_win_ctx.mouse_buttons_released));
    g_win_ctx.mouse_wheel = 0.0f;

    glfwPollEvents();

    g_win_ctx.mouse_delta = (Vector2){
        g_win_ctx.mouse_pos.x - g_win_ctx.last_mouse_pos.x,
        g_win_ctx.mouse_pos.y - g_win_ctx.last_mouse_pos.y
    };
    g_win_ctx.last_mouse_pos = g_win_ctx.mouse_pos;

    // Delta Time calculation
    double now = glfwGetTime();
    g_win_ctx.frame_time = (float)(now - g_win_ctx.last_frame_time);
    g_win_ctx.last_frame_time = now;

    // FPS calculation
    g_win_ctx.frame_counter++;
    if (now - g_win_ctx.last_fps_update_time >= 1.0) {
        g_win_ctx.current_fps = g_win_ctx.frame_counter;
        g_win_ctx.frame_counter = 0;
        g_win_ctx.last_fps_update_time = now;
    }
}

void internal_window_swap_buffers(void) {
    glfwSwapBuffers(g_win_ctx.handle);

    // Target FPS framing delay if needed
    if (g_win_ctx.target_fps > 0) {
        double elapsed = glfwGetTime() - g_win_ctx.last_frame_time;
        while (elapsed < g_win_ctx.target_frame_duration) {
            double wait_time = g_win_ctx.target_frame_duration - elapsed;
            if (wait_time > 0.001) {
                glfwWaitEventsTimeout(wait_time * 0.5);
            }
            elapsed = glfwGetTime() - g_win_ctx.last_frame_time;
        }
    }
}

// ----------------------------------------------------------------------------------
// Input API Implementation
// ----------------------------------------------------------------------------------

bool is_key_down(int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    return g_win_ctx.keys_down[key];
}

bool is_key_pressed(int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    return g_win_ctx.keys_pressed[key];
}

bool is_key_released(int key) {
    if (key < 0 || key >= MAX_KEYS) return false;
    return g_win_ctx.keys_released[key];
}

bool is_mouse_button_down(int button) {
    if (button < 0 || button >= MAX_MOUSE_BUTTONS) return false;
    return g_win_ctx.mouse_buttons_down[button];
}

bool is_mouse_button_pressed(int button) {
    if (button < 0 || button >= MAX_MOUSE_BUTTONS) return false;
    return g_win_ctx.mouse_buttons_pressed[button];
}

bool is_mouse_button_released(int button) {
    if (button < 0 || button >= MAX_MOUSE_BUTTONS) return false;
    return g_win_ctx.mouse_buttons_released[button];
}

Vector2 get_mouse_position(void) {
    return g_win_ctx.mouse_pos;
}

Vector2 get_mouse_delta(void) {
    return g_win_ctx.mouse_delta;
}

float get_mouse_wheel_move(void) {
    return g_win_ctx.mouse_wheel;
}
