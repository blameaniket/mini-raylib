#include "maths.h"
#include <string.h>

Vector2 vector2_zero(void) {
    return (Vector2){ 0.0f, 0.0f };
}

Vector2 vector2_create(float x, float y) {
    return (Vector2){ x, y };
}

Vector2 vector2_add(Vector2 v1, Vector2 v2) {
    return (Vector2){ v1.x + v2.x, v1.y + v2.y };
}

Vector2 vector2_subtract(Vector2 v1, Vector2 v2) {
    return (Vector2){ v1.x - v2.x, v1.y - v2.y };
}

Vector2 vector2_scale(Vector2 v, float scale) {
    return (Vector2){ v.x * scale, v.y * scale };
}

float vector2_length(Vector2 v) {
    return sqrtf(v.x * v.x + v.y * v.y);
}

Vector2 vector2_normalize(Vector2 v) {
    float len = vector2_length(v);
    if (len > 0.000001f) {
        return (Vector2){ v.x / len, v.y / len };
    }
    return (Vector2){ 0.0f, 0.0f };
}

Vector3 vector3_zero(void) {
    return (Vector3){ 0.0f, 0.0f, 0.0f };
}

Vector3 vector3_create(float x, float y, float z) {
    return (Vector3){ x, y, z };
}

Vector3 vector3_add(Vector3 v1, Vector3 v2) {
    return (Vector3){ v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

Vector3 vector3_subtract(Vector3 v1, Vector3 v2) {
    return (Vector3){ v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vector3 vector3_scale(Vector3 v, float scale) {
    return (Vector3){ v.x * scale, v.y * scale, v.z * scale };
}

float vector3_length(Vector3 v) {
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vector3 vector3_normalize(Vector3 v) {
    float len = vector3_length(v);
    if (len > 0.000001f) {
        return (Vector3){ v.x / len, v.y / len, v.z / len };
    }
    return (Vector3){ 0.0f, 0.0f, 0.0f };
}

Vector3 vector3_cross_product(Vector3 v1, Vector3 v2) {
    return (Vector3){
        v1.y * v2.z - v1.z * v2.y,
        v1.z * v2.x - v1.x * v2.z,
        v1.x * v2.y - v1.y * v2.x
    };
}

float vector3_dot_product(Vector3 v1, Vector3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vector4 color_to_vector4(Color color) {
    return (Vector4){
        (float)color.r / 255.0f,
        (float)color.g / 255.0f,
        (float)color.b / 255.0f,
        (float)color.a / 255.0f
    };
}

Matrix matrix_identity(void) {
    Matrix result = { 0 };
    result.m[0] = 1.0f;
    result.m[5] = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;
    return result;
}

// Column-major matrix multiplication: result = left * right
Matrix matrix_multiply(Matrix left, Matrix right) {
    Matrix res = { 0 };
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res.m[i + j * 4] =
                left.m[i + 0 * 4] * right.m[0 + j * 4] +
                left.m[i + 1 * 4] * right.m[1 + j * 4] +
                left.m[i + 2 * 4] * right.m[2 + j * 4] +
                left.m[i + 3 * 4] * right.m[3 + j * 4];
        }
    }
    return res;
}

Matrix matrix_translate(float x, float y, float z) {
    Matrix res = matrix_identity();
    res.m[12] = x;
    res.m[13] = y;
    res.m[14] = z;
    return res;
}

Matrix matrix_rotate_x(float angle_rad) {
    Matrix res = matrix_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    res.m[5] = c;
    res.m[6] = s;
    res.m[9] = -s;
    res.m[10] = c;
    return res;
}

Matrix matrix_rotate_y(float angle_rad) {
    Matrix res = matrix_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    res.m[0] = c;
    res.m[2] = -s;
    res.m[8] = s;
    res.m[10] = c;
    return res;
}

Matrix matrix_rotate_z(float angle_rad) {
    Matrix res = matrix_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    res.m[0] = c;
    res.m[1] = s;
    res.m[4] = -s;
    res.m[5] = c;
    return res;
}

Matrix matrix_rotate(Vector3 axis, float angle_rad) {
    Matrix res = matrix_identity();
    axis = vector3_normalize(axis);
    float x = axis.x, y = axis.y, z = axis.z;
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    float t = 1.0f - c;

    res.m[0] = t * x * x + c;
    res.m[1] = t * x * y + s * z;
    res.m[2] = t * x * z - s * y;

    res.m[4] = t * x * y - s * z;
    res.m[5] = t * y * y + c;
    res.m[6] = t * y * z + s * x;

    res.m[8] = t * x * z + s * y;
    res.m[9] = t * y * z - s * x;
    res.m[10] = t * z * z + c;

    return res;
}

Matrix matrix_scale(float x, float y, float z) {
    Matrix res = matrix_identity();
    res.m[0] = x;
    res.m[5] = y;
    res.m[10] = z;
    return res;
}

Matrix matrix_ortho(float left, float right, float bottom, float top, float near_plane, float far_plane) {
    Matrix res = matrix_identity();
    float rl = right - left;
    float tb = top - bottom;
    float fn = far_plane - near_plane;

    res.m[0] = 2.0f / rl;
    res.m[5] = 2.0f / tb;
    res.m[10] = -2.0f / fn;

    res.m[12] = -(right + left) / rl;
    res.m[13] = -(top + bottom) / tb;
    res.m[14] = -(far_plane + near_plane) / fn;

    return res;
}

Matrix matrix_perspective(float fov_rad, float aspect, float near_plane, float far_plane) {
    Matrix res = { 0 };
    float top = near_plane * tanf(fov_rad * 0.5f);
    float right = top * aspect;

    res.m[0] = near_plane / right;
    res.m[5] = near_plane / top;
    res.m[10] = -(far_plane + near_plane) / (far_plane - near_plane);
    res.m[11] = -1.0f;
    res.m[14] = -(2.0f * far_plane * near_plane) / (far_plane - near_plane);
    return res;
}

Matrix matrix_look_at(Vector3 eye, Vector3 target, Vector3 up) {
    Vector3 f = vector3_normalize(vector3_subtract(target, eye));
    Vector3 r = vector3_normalize(vector3_cross_product(f, up));
    Vector3 u = vector3_cross_product(r, f);

    Matrix res = matrix_identity();
    res.m[0] = r.x;
    res.m[4] = r.y;
    res.m[8] = r.z;

    res.m[1] = u.x;
    res.m[5] = u.y;
    res.m[9] = u.z;

    res.m[2] = -f.x;
    res.m[6] = -f.y;
    res.m[10] = -f.z;

    res.m[12] = -vector3_dot_product(r, eye);
    res.m[13] = -vector3_dot_product(u, eye);
    res.m[14] = vector3_dot_product(f, eye);

    return res;
}
