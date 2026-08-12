#include "app.h"

int main() {
    App app = app_create((AppConfig){
        .width = 1200,
        .height = 700,
        .title = "sample opengl app",
        .target_fps = 60,
        .bg_color = (Color){ 30, 32, 40, 255 }
    });

    return app_run(&app);
}
