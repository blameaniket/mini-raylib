#ifndef APP_H
#define APP_H

#include "maths.h"
#include "window.h"
#include "camera.h"
#include "renderer.h"

// Forward declaration
typedef struct App App;

// Function pointer types for customizable application callbacks
typedef void (*AppCallback)(App *app);
typedef void (*AppUpdateCallback)(App *app, float dt);

// Configuration options for initializing the engine / app
typedef struct AppConfig {
    int width;
    int height;
    const char *title;
    int target_fps;
    Color bg_color;
} AppConfig;

// Application game state logic
typedef struct AppState {
    Camera3D camera;
    float cube_rotation;
    float pyramid_height;
    bool show_grid;
    bool show_wires;
    Color cube_color;
} AppState;

// Engine Application Structure
struct App {
    AppConfig config;
    AppState state;
    bool is_running;

    // Optional lifecycle callbacks (defaults provided if NULL)
    AppCallback on_init;
    AppUpdateCallback on_update;
    AppCallback on_render_3d;
    AppCallback on_render_2d;
    AppCallback on_cleanup;
};

// Engine API Functions
App app_create(AppConfig config);
void app_init(App *app, int width, int height, const char *title, int target_fps);
int app_run(App *app);
void app_quit(App *app);

#endif // APP_H
