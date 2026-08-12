#include "camera.h"
#include "window.h"
#include <math.h>

Matrix get_camera_matrix_2d(Camera2D camera, int screen_width, int screen_height) {
    (void)screen_width; (void)screen_height;

    Matrix mat_scale = matrix_scale(camera.zoom, camera.zoom, 1.0f);
    Matrix mat_rotation = matrix_rotate_z(camera.rotation * DEG2RAD);
    Matrix mat_translation_target = matrix_translate(-camera.target.x, -camera.target.y, 0.0f);
    Matrix mat_translation_offset = matrix_translate(camera.offset.x, camera.offset.y, 0.0f);

    // Order: offset * rotation * scale * -target
    Matrix mat_transform = matrix_multiply(mat_translation_offset, matrix_multiply(mat_rotation, matrix_multiply(mat_scale, mat_translation_target)));
    return mat_transform;
}

Matrix get_camera_matrix_3d(Camera3D camera) {
    return matrix_look_at(camera.position, camera.target, camera.up);
}

void update_camera(Camera3D *camera, CameraMode mode) {
    if (!camera) return;

    float dt = get_frame_time();
    Vector2 mouse_delta = get_mouse_delta();
    float wheel = get_mouse_wheel_move();

    if (mode == CAMERA_ORBITAL) {
        Vector3 forward = vector3_subtract(camera->position, camera->target);
        float radius = vector3_length(forward);

        float theta = atan2f(forward.x, forward.z);
        float phi = atan2f(forward.y, sqrtf(forward.x * forward.x + forward.z * forward.z));

        // Mouse drag with right button or left shift
        if (is_mouse_button_down(MOUSE_BUTTON_RIGHT) || (is_mouse_button_down(MOUSE_BUTTON_LEFT) && is_key_down(KEY_LEFT_SHIFT))) {
            theta -= mouse_delta.x * 0.005f;
            phi += mouse_delta.y * 0.005f;

            // Clamp phi to avoid gimble flip
            if (phi > 89.0f * DEG2RAD) phi = 89.0f * DEG2RAD;
            if (phi < -89.0f * DEG2RAD) phi = -89.0f * DEG2RAD;
        }

        // Zoom with wheel
        if (wheel != 0.0f) {
            radius -= wheel * 0.5f;
            if (radius < 1.0f) radius = 1.0f;
        }

        // Key controls for orbit angle
        if (is_key_down(KEY_LEFT)) theta += 1.5f * dt;
        if (is_key_down(KEY_RIGHT)) theta -= 1.5f * dt;
        if (is_key_down(KEY_UP)) phi += 1.5f * dt;
        if (is_key_down(KEY_DOWN)) phi -= 1.5f * dt;

        camera->position.x = camera->target.x + radius * cosf(phi) * sinf(theta);
        camera->position.y = camera->target.y + radius * sinf(phi);
        camera->position.z = camera->target.z + radius * cosf(phi) * cosf(theta);

    } else if (mode == CAMERA_FREE || mode == CAMERA_FIRST_PERSON) {
        Vector3 forward = vector3_normalize(vector3_subtract(camera->target, camera->position));
        Vector3 right = vector3_normalize(vector3_cross_product(forward, camera->up));

        float speed = 5.0f * dt;
        if (is_key_down(KEY_LEFT_SHIFT)) speed *= 2.5f;

        if (is_key_down(KEY_W)) camera->position = vector3_add(camera->position, vector3_scale(forward, speed));
        if (is_key_down(KEY_S)) camera->position = vector3_subtract(camera->position, vector3_scale(forward, speed));
        if (is_key_down(KEY_D)) camera->position = vector3_add(camera->position, vector3_scale(right, speed));
        if (is_key_down(KEY_A)) camera->position = vector3_subtract(camera->position, vector3_scale(right, speed));

        if (mode == CAMERA_FREE) {
            if (is_key_down(KEY_E)) camera->position.y += speed;
            if (is_key_down(KEY_Q)) camera->position.y -= speed;
        }

        if (is_mouse_button_down(MOUSE_BUTTON_RIGHT)) {
            // Mouse look
            float turn_sensitivity = 0.003f;
            // Yaw
            Matrix mat_yaw = matrix_rotate_y(-mouse_delta.x * turn_sensitivity);
            // Pitch
            Matrix mat_pitch = matrix_rotate(right, -mouse_delta.y * turn_sensitivity);

            Matrix rotation = matrix_multiply(mat_yaw, mat_pitch);

            Vector3 new_forward = {
                rotation.m[0] * forward.x + rotation.m[4] * forward.y + rotation.m[8] * forward.z,
                rotation.m[1] * forward.x + rotation.m[5] * forward.y + rotation.m[9] * forward.z,
                rotation.m[2] * forward.x + rotation.m[6] * forward.y + rotation.m[10] * forward.z
            };

            camera->target = vector3_add(camera->position, new_forward);
        } else {
            camera->target = vector3_add(camera->position, forward);
        }
    }
}
