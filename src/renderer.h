#ifndef RENDERER_H
#define RENDERER_H

#include "maths.h"
#include "camera.h"

// ----------------------------------------------------------------------------------
// Renderer API Declarations
// ----------------------------------------------------------------------------------

void begin_drawing(void);
void end_drawing(void);
void clear_background(Color color);

void begin_mode_2d(Camera2D camera);
void end_mode_2d(void);

void begin_mode_3d(Camera3D camera);
void end_mode_3d(void);

// 2D Shape Drawing Primitives
void draw_pixel(int x, int y, Color color);
void draw_line(int start_x, int start_y, int end_x, int end_y, Color color);
void draw_triangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);
void draw_rectangle(int x, int y, int width, int height, Color color);
void draw_rectangle_lines(int x, int y, int width, int height, Color color);
void draw_circle(int center_x, int center_y, float radius, Color color);
void draw_circle_lines(int center_x, int center_y, float radius, Color color);
void draw_poly(Vector2 center, int sides, float radius, float rotation, Color color);

// 2D Simple Text Drawing
void draw_text(const char *text, int pos_x, int pos_y, int font_size, Color color);

// 3D Shape Drawing Primitives
void draw_line_3d(Vector3 start_pos, Vector3 end_pos, Color color);
void draw_point_3d(Vector3 position, Color color);
void draw_triangle_3d(Vector3 v1, Vector3 v2, Vector3 v3, Color color);
void draw_cube(Vector3 position, float width, float height, float length, Color color);
void draw_cube_wires(Vector3 position, float width, float height, float length, Color color);
void draw_sphere_wires(Vector3 center_pos, float radius, int rings, int slices, Color color);
void draw_grid(int slices, float spacing);

#endif // RENDERER_H
