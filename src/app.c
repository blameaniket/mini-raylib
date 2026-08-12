#include "app.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static void default_app_init(App *app) {
    AppState *state = &app->state;

    // Setup 3D Camera
    state->camera.position = (Vector3){ 6.0f, 6.0f, 6.0f };
    state->camera.target = (Vector3){ 0.0f, 1.0f, 0.0f };
    state->camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    state->camera.fovy = 45.0f;
    state->camera.projection = CAMERA_PERSPECTIVE;

    state->cube_rotation = 0.0f;
    state->pyramid_height = 0.0f;
    state->show_grid = true;
    state->show_wires = true;
    state->cube_color = BLACK;
}

static void default_app_update(App *app, float dt) {
    AppState *state = &app->state;

    // Camera orbital controls
    update_camera(&state->camera, CAMERA_ORBITAL);

    // Rotate cube over time
    state->cube_rotation += 45.0f * dt;
    if (state->cube_rotation >= 360.0f) state->cube_rotation -= 360.0f;

    // Bob pyramid up and down
    state->pyramid_height = sinf((float)get_time() * 2.0f) * 0.5f;

    // Toggle options with keys
    if (is_key_pressed(KEY_G)) state->show_grid = !state->show_grid;
    if (is_key_pressed(KEY_W)) state->show_wires = !state->show_wires;
    if (is_key_pressed(KEY_SPACE)) {
        state->cube_color = (Color){ (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255 };
    }
}

static void default_app_render_3d(App *app) {
    AppState *state = &app->state;

    begin_mode_3d(state->camera);

    // Draw coordinate grid
    if (state->show_grid) {
        draw_grid(20, 1.0f);
    }

    // Draw central rotating cube
    Vector3 cube_pos = { 0.0f, 1.0f, 0.0f };
    if (state->show_wires) {
        draw_cube_wires(cube_pos, 2.0f, 2.0f, 2.0f, state->cube_color);
    } else {
        draw_cube(cube_pos, 2.0f, 2.0f, 2.0f, state->cube_color);
        draw_cube_wires(cube_pos, 2.02f, 2.02f, 2.02f, BLACK);
    }

    // Draw animated floating pyramid / triangle
    Vector3 p_top = { 0.0f, 3.5f + state->pyramid_height, 0.0f };
    Vector3 p1 = { -1.0f, 2.5f + state->pyramid_height, 1.0f };
    Vector3 p2 = {  1.0f, 2.5f + state->pyramid_height, 1.0f };
    Vector3 p3 = {  0.0f, 2.5f + state->pyramid_height, -1.0f };

    draw_triangle_3d(p_top, p1, p2, GOLD);
    draw_triangle_3d(p_top, p2, p3, ORANGE);
    draw_triangle_3d(p_top, p3, p1, RED);

    // Draw surrounding decorative spheres
    draw_sphere_wires((Vector3){ -3.0f, 0.5f, -3.0f }, 0.8f, 12, 12, LIME);
    draw_sphere_wires((Vector3){  3.0f, 0.5f,  3.0f }, 0.8f, 12, 12, VIOLET);

    end_mode_3d();
}

static void default_app_render_2d(App *app) {
    (void)app;

    // Draw semi-transparent UI card header
    draw_rectangle(10, 10, 320, 150, (Color){ 20, 24, 35, 220 });
    draw_rectangle_lines(10, 10, 320, 150, SKYBLUE);

    // Render Text Info & FPS
    char fps_buf[64];
    snprintf(fps_buf, sizeof(fps_buf), "FPS: %d (%.2f ms)", get_fps(), get_frame_time() * 1000.0f);
    draw_text(fps_buf, 25, 25, 16, GREEN);

    draw_text("OpenGL Mini-Raylib App", 25, 45, 16, WHITE);
    draw_text("[Right Drag / Arrows] Orbit Camera", 25, 70, 8, LIGHTGRAY);
    draw_text("[Mouse Wheel] Zoom Camera", 25, 85, 8, LIGHTGRAY);
    draw_text("[G] Toggle Grid | [W] Wireframe", 25, 100, 8, LIGHTGRAY);
    draw_text("[Space] Change Cube Color", 25, 115, 8, LIGHTGRAY);

    // Interactive Button Demo
    Vector2 mouse_pos = get_mouse_position();
    bool hover_btn = (mouse_pos.x >= 25 && mouse_pos.x <= 200 && mouse_pos.y >= 130 && mouse_pos.y <= 150);
    Color btn_color = hover_btn ? MAROON : RED;
    draw_rectangle(25, 130, 175, 22, btn_color);
    draw_text("Click Me (Interactive)", 35, 137, 8, WHITE);
}

App app_create(AppConfig config) {
    App app = {0};
    app.config = config;

    if (app.config.width <= 0) app.config.width = 1200;
    if (app.config.height <= 0) app.config.height = 700;
    if (app.config.title == NULL) app.config.title = "Mini-Raylib Application";
    if (app.config.target_fps <= 0) app.config.target_fps = 60;
    if (app.config.bg_color.a == 0) app.config.bg_color = (Color){ 30, 32, 40, 255 };

    app.is_running = false;
    app.on_init = default_app_init;
    app.on_update = default_app_update;
    app.on_render_3d = default_app_render_3d;
    app.on_render_2d = default_app_render_2d;
    app.on_cleanup = NULL;

    return app;
}

void app_init(App *app, int width, int height, const char *title, int target_fps) {
    if (!app) return;
    *app = app_create((AppConfig){
        .width = width,
        .height = height,
        .title = title,
        .target_fps = target_fps,
        .bg_color = (Color){ 30, 32, 40, 255 }
    });
}

void app_quit(App *app) {
    if (app) app->is_running = false;
}

int app_run(App *app) {
    if (!app) return 1;

    // 1. Initialize Window & Context
    init_window(app->config.width, app->config.height, app->config.title);
    set_target_fps(app->config.target_fps);

    app->is_running = true;

    // 2. Application Custom / Default Init
    if (app->on_init) {
        app->on_init(app);
    }

    // 3. Main Engine Loop
    while (!window_should_close() && app->is_running) {
        float dt = get_frame_time();

        if (app->on_update) {
            app->on_update(app, dt);
        }

        begin_drawing();
            clear_background(app->config.bg_color);

            if (app->on_render_3d) {
                app->on_render_3d(app);
            }

            if (app->on_render_2d) {
                app->on_render_2d(app);
            }

        end_drawing();
    }

    // 4. Application Cleanup
    if (app->on_cleanup) {
        app->on_cleanup(app);
    }

    close_window();
    return 0;
}
