#ifndef CAMERA_H
#define CAMERA_H

#include "maths.h"

// ----------------------------------------------------------------------------------
// Camera Types Definitions
// ----------------------------------------------------------------------------------

typedef struct Camera2D {
    Vector2 offset;     // Camera offset (displacement from screen center)
    Vector2 target;     // Camera target point
    float rotation;     // Camera rotation in degrees
    float zoom;         // Camera zoom (scale), should be > 0.0f
} Camera2D;

typedef enum {
    CAMERA_PERSPECTIVE = 0,
    CAMERA_ORTHOGRAPHIC
} CameraProjection;

typedef struct Camera3D {
    Vector3 position;   // Camera position
    Vector3 target;     // Camera target (look-at point)
    Vector3 up;         // Camera up vector (rotation orientation)
    float fovy;         // Camera field-of-view Y in degrees (or size if orthographic)
    int projection;     // Camera projection mode (CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC)
} Camera3D;

typedef enum {
    CAMERA_CUSTOM = 0,
    CAMERA_FREE,
    CAMERA_ORBITAL,
    CAMERA_FIRST_PERSON
} CameraMode;

// ----------------------------------------------------------------------------------
// Camera Functions
// ----------------------------------------------------------------------------------

Matrix get_camera_matrix_2d(Camera2D camera, int screen_width, int screen_height);
Matrix get_camera_matrix_3d(Camera3D camera);

void update_camera(Camera3D *camera, CameraMode mode);

#endif // CAMERA_H
