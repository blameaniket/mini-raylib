#ifndef MATHS_H
#define MATHS_H

#include <math.h>
#include <stdbool.h>

#define PI 3.14159265358979323846f
#define DEG2RAD (PI / 180.0f)
#define RAD2DEG (180.0f / PI)

// ----------------------------------------------------------------------------------
// Types and Structures Definitions
// ----------------------------------------------------------------------------------

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

typedef struct Vector4 {
    float x;
    float y;
    float z;
    float w;
} Vector4;

// OpenGL standard 4x4 matrix (column-major: m[0..3] is col 0, m[4..7] is col 1, etc.)
typedef struct Matrix {
    float m[16];
} Matrix;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

// ----------------------------------------------------------------------------------
// Color Constants Definitions (Raylib style)
// ----------------------------------------------------------------------------------

#define LIGHTGRAY  (Color){ 200, 200, 200, 255 }
#define GRAY       (Color){ 130, 130, 130, 255 }
#define DARKGRAY   (Color){ 80, 80, 80, 255 }
#define YELLOW     (Color){ 253, 249, 0, 255 }
#define GOLD       (Color){ 255, 203, 0, 255 }
#define ORANGE     (Color){ 255, 161, 0, 255 }
#define PINK       (Color){ 255, 109, 194, 255 }
#define RED        (Color){ 230, 41, 55, 255 }
#define MAROON     (Color){ 190, 33, 55, 255 }
#define GREEN      (Color){ 0, 228, 48, 255 }
#define LIME       (Color){ 0, 158, 47, 255 }
#define DARKGREEN  (Color){ 0, 117, 44, 255 }
#define SKYBLUE    (Color){ 102, 191, 255, 255 }
#define BLUE       (Color){ 0, 121, 241, 255 }
#define DARKBLUE   (Color){ 0, 82, 172, 255 }
#define PURPLE     (Color){ 200, 122, 255, 255 }
#define VIOLET     (Color){ 135, 60, 190, 255 }
#define WHITE      (Color){ 255, 255, 255, 255 }
#define BLACK      (Color){ 0, 0, 0, 255 }
#define BLANK      (Color){ 0, 0, 0, 0 }
#define MAGENTA    (Color){ 255, 0, 255, 255 }
#define RAYWHITE   (Color){ 245, 245, 245, 255 }

// ----------------------------------------------------------------------------------
// Vector Math Functions
// ----------------------------------------------------------------------------------

Vector2 vector2_zero(void);
Vector2 vector2_create(float x, float y);
Vector2 vector2_add(Vector2 v1, Vector2 v2);
Vector2 vector2_subtract(Vector2 v1, Vector2 v2);
Vector2 vector2_scale(Vector2 v, float scale);
float vector2_length(Vector2 v);
Vector2 vector2_normalize(Vector2 v);

Vector3 vector3_zero(void);
Vector3 vector3_create(float x, float y, float z);
Vector3 vector3_add(Vector3 v1, Vector3 v2);
Vector3 vector3_subtract(Vector3 v1, Vector3 v2);
Vector3 vector3_scale(Vector3 v, float scale);
float vector3_length(Vector3 v);
Vector3 vector3_normalize(Vector3 v);
Vector3 vector3_cross_product(Vector3 v1, Vector3 v2);
float vector3_dot_product(Vector3 v1, Vector3 v2);

Vector4 color_to_vector4(Color color);

// ----------------------------------------------------------------------------------
// Matrix Functions
// ----------------------------------------------------------------------------------

Matrix matrix_identity(void);
Matrix matrix_multiply(Matrix left, Matrix right);
Matrix matrix_translate(float x, float y, float z);
Matrix matrix_rotate_x(float angle_rad);
Matrix matrix_rotate_y(float angle_rad);
Matrix matrix_rotate_z(float angle_rad);
Matrix matrix_rotate(Vector3 axis, float angle_rad);
Matrix matrix_scale(float x, float y, float z);
Matrix matrix_ortho(float left, float right, float bottom, float top, float near_plane, float far_plane);
Matrix matrix_perspective(float fov_rad, float aspect, float near_plane, float far_plane);
Matrix matrix_look_at(Vector3 eye, Vector3 target, Vector3 up);

#endif // MATHS_H
