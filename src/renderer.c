#include "renderer.h"
#include "window.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern void internal_window_poll_events(void);
extern void internal_window_swap_buffers(void);

#define MAX_BATCH_VERTICES 24000

typedef struct Vertex {
    float x, y, z;
    float r, g, b, a;
} Vertex;

typedef enum {
    MODE_TRIANGLES = 0,
    MODE_LINES
} DrawMode;

typedef struct RenderBatch {
    GLuint vao;
    GLuint vbo;
    GLuint shader_program;
    GLint mvp_loc;

    Vertex vertices[MAX_BATCH_VERTICES];
    int vertex_count;
    DrawMode current_draw_mode;

    Matrix projection;
    Matrix view;
    Matrix model;
    Matrix mvp;
    bool mvp_dirty;

    int render_mode_2d_3d; // 0 = default 2D screen space, 1 = 2D camera, 2 = 3D camera
    Camera2D camera_2d;
    Camera3D camera_3d;
} RenderBatch;

static RenderBatch g_batch = { 0 };

// ----------------------------------------------------------------------------------
// Embedded 8x8 ASCII Font Bitmap (Basic printable ASCII 32..126)
// ----------------------------------------------------------------------------------
static const unsigned char font8x8_basic[95][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}, // ' ' (32)
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00}, // '!'
    {0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00}, // '"'
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00}, // '#'
    {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00}, // '$'
    {0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00}, // '%'
    {0x1C,0x36,0x1C,0x0E,0x3B,0x33,0x6E,0x00}, // '&'
    {0x06,0x06,0x0C,0x00,0x00,0x00,0x00,0x00}, // '\''
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00}, // '('
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00}, // ')'
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00}, // '*'
    {0x00,0x0C,0x0C,0x3E,0x0C,0x0C,0x00,0x00}, // '+'
    {0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x18}, // ','
    {0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x00}, // '-'
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00}, // '.'
    {0x00,0x03,0x06,0x0C,0x18,0x30,0x60,0x00}, // '/'
    {0x3E,0x63,0x67,0x6F,0x7B,0x63,0x3E,0x00}, // '0'
    {0x0C,0x1C,0x0C,0x0C,0x0C,0x0C,0x3E,0x00}, // '1'
    {0x3E,0x63,0x06,0x1C,0x30,0x63,0x7F,0x00}, // '2'
    {0x3E,0x63,0x06,0x1C,0x06,0x63,0x3E,0x00}, // '3'
    {0x0E,0x1E,0x36,0x66,0x7F,0x06,0x0F,0x00}, // '4'
    {0x7F,0x60,0x7E,0x03,0x03,0x63,0x3E,0x00}, // '5'
    {0x1C,0x30,0x60,0x7E,0x63,0x63,0x3E,0x00}, // '6'
    {0x7F,0x63,0x06,0x0C,0x18,0x18,0x18,0x00}, // '7'
    {0x3E,0x63,0x63,0x3E,0x63,0x63,0x3E,0x00}, // '8'
    {0x3E,0x63,0x63,0x3F,0x03,0x06,0x3C,0x00}, // '9'
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00}, // ':'
    {0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x30}, // ';'
    {0x0C,0x18,0x30,0x60,0x30,0x18,0x0C,0x00}, // '<'
    {0x00,0x00,0x3E,0x00,0x3E,0x00,0x00,0x00}, // '='
    {0x30,0x18,0x0C,0x06,0x0C,0x18,0x30,0x00}, // '>'
    {0x3E,0x63,0x06,0x0C,0x18,0x00,0x18,0x00}, // '?'
    {0x3E,0x63,0x6F,0x6B,0x6F,0x60,0x3E,0x00}, // '@'
    {0x18,0x3C,0x66,0x66,0x7E,0x66,0x66,0x00}, // 'A'
    {0x7E,0x33,0x33,0x3E,0x33,0x33,0x7E,0x00}, // 'B'
    {0x1E,0x33,0x60,0x60,0x60,0x33,0x1E,0x00}, // 'C'
    {0x7C,0x36,0x33,0x33,0x33,0x36,0x7C,0x00}, // 'D'
    {0x7F,0x31,0x34,0x3C,0x34,0x30,0x7F,0x00}, // 'E'
    {0x7F,0x31,0x34,0x3C,0x34,0x30,0x30,0x00}, // 'F'
    {0x1E,0x33,0x60,0x67,0x63,0x33,0x1F,0x00}, // 'G'
    {0x66,0x66,0x66,0x7E,0x66,0x66,0x66,0x00}, // 'H'
    {0x3C,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 'I'
    {0x0F,0x06,0x06,0x06,0x66,0x66,0x3C,0x00}, // 'J'
    {0x66,0x36,0x1E,0x1C,0x1E,0x36,0x66,0x00}, // 'K'
    {0x30,0x30,0x30,0x30,0x30,0x30,0x7E,0x00}, // 'L'
    {0x63,0x77,0x7F,0x6B,0x63,0x63,0x63,0x00}, // 'M'
    {0x63,0x73,0x7B,0x6F,0x67,0x63,0x63,0x00}, // 'N'
    {0x3C,0x66,0x66,0x66,0x66,0x66,0x3C,0x00}, // 'O'
    {0x7E,0x33,0x33,0x3E,0x30,0x30,0x30,0x00}, // 'P'
    {0x3C,0x66,0x66,0x66,0x6E,0x3C,0x0E,0x00}, // 'Q'
    {0x7E,0x33,0x33,0x3E,0x1B,0x33,0x66,0x00}, // 'R'
    {0x3E,0x63,0x30,0x1E,0x03,0x63,0x3E,0x00}, // 'S'
    {0x7E,0x5A,0x18,0x18,0x18,0x18,0x3C,0x00}, // 'T'
    {0x66,0x66,0x66,0x66,0x66,0x66,0x3E,0x00}, // 'U'
    {0x66,0x66,0x66,0x66,0x66,0x3C,0x18,0x00}, // 'V'
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00}, // 'W'
    {0x63,0x63,0x36,0x1C,0x36,0x63,0x63,0x00}, // 'X'
    {0x66,0x66,0x66,0x3C,0x18,0x18,0x18,0x00}, // 'Y'
    {0x7F,0x06,0x0C,0x18,0x30,0x60,0x7F,0x00}, // 'Z'
    {0x3C,0x30,0x30,0x30,0x30,0x30,0x3C,0x00}, // '['
    {0x00,0x60,0x30,0x18,0x0C,0x06,0x03,0x00}, // '\'
    {0x3C,0x0C,0x0C,0x0C,0x0C,0x0C,0x3C,0x00}, // ']'
    {0x18,0x3C,0x66,0x00,0x00,0x00,0x00,0x00}, // '^'
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF}, // '_'
    {0x30,0x18,0x0C,0x00,0x00,0x00,0x00,0x00}, // '`'
    {0x00,0x00,0x3C,0x06,0x3E,0x66,0x3E,0x00}, // 'a'
    {0x30,0x30,0x3E,0x33,0x33,0x33,0x3E,0x00}, // 'b'
    {0x00,0x00,0x1E,0x33,0x30,0x33,0x1E,0x00}, // 'c'
    {0x06,0x06,0x3E,0x66,0x66,0x66,0x3E,0x00}, // 'd'
    {0x00,0x00,0x3E,0x66,0x7E,0x30,0x1E,0x00}, // 'e'
    {0x0E,0x18,0x3E,0x18,0x18,0x18,0x18,0x00}, // 'f'
    {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x3C}, // 'g'
    {0x30,0x30,0x3E,0x33,0x33,0x33,0x33,0x00}, // 'h'
    {0x18,0x00,0x38,0x18,0x18,0x18,0x3C,0x00}, // 'i'
    {0x06,0x00,0x0E,0x06,0x06,0x66,0x66,0x3C}, // 'j'
    {0x30,0x30,0x36,0x3C,0x36,0x33,0x33,0x00}, // 'k'
    {0x38,0x18,0x18,0x18,0x18,0x18,0x3C,0x00}, // 'l'
    {0x00,0x00,0x66,0x7F,0x6B,0x63,0x63,0x00}, // 'm'
    {0x00,0x00,0x7C,0x66,0x66,0x66,0x66,0x00}, // 'n'
    {0x00,0x00,0x3C,0x66,0x66,0x66,0x3C,0x00}, // 'o'
    {0x00,0x00,0x3E,0x33,0x33,0x3E,0x30,0x30}, // 'p'
    {0x00,0x00,0x3E,0x66,0x66,0x3E,0x06,0x06}, // 'q'
    {0x00,0x00,0x36,0x3C,0x30,0x30,0x30,0x00}, // 'r'
    {0x00,0x00,0x3E,0x30,0x1E,0x03,0x3E,0x00}, // 's'
    {0x18,0x18,0x7E,0x18,0x18,0x18,0x0E,0x00}, // 't'
    {0x00,0x00,0x33,0x33,0x33,0x33,0x1E,0x00}, // 'u'
    {0x00,0x00,0x66,0x66,0x66,0x3C,0x18,0x00}, // 'v'
    {0x00,0x00,0x63,0x63,0x6B,0x7F,0x36,0x00}, // 'w'
    {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00}, // 'x'
    {0x00,0x00,0x66,0x66,0x66,0x3E,0x06,0x3C}, // 'y'
    {0x00,0x00,0x7F,0x0C,0x18,0x30,0x7F,0x00}, // 'z'
    {0x0E,0x18,0x18,0x30,0x18,0x18,0x0E,0x00}, // '{'
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00}, // '|'
    {0x70,0x18,0x18,0x0C,0x18,0x18,0x70,0x00}, // '}'
    {0x3B,0x6E,0x00,0x00,0x00,0x00,0x00,0x00}, // '~'
};

// ----------------------------------------------------------------------------------
// Internal Shader Compilation & Setup
// ----------------------------------------------------------------------------------

static const char *vs_source =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec4 aColor;\n"
    "out vec4 FragColor;\n"
    "uniform mat4 u_MVP;\n"
    "void main() {\n"
    "    FragColor = aColor;\n"
    "    gl_Position = u_MVP * vec4(aPos, 1.0);\n"
    "}\n";

static const char *fs_source =
    "#version 330 core\n"
    "in vec4 FragColor;\n"
    "out vec4 FinalColor;\n"
    "void main() {\n"
    "    FinalColor = FragColor;\n"
    "}\n";

static GLuint compile_shader(GLenum type, const char *src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "[ERROR] Shader compilation failed:\n%s\n", info_log);
    }
    return shader;
}

static GLuint create_program(const char *vs, const char *fs) {
    GLuint v_shader = compile_shader(GL_VERTEX_SHADER, vs);
    GLuint f_shader = compile_shader(GL_FRAGMENT_SHADER, fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, v_shader);
    glAttachShader(program, f_shader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "[ERROR] Shader linking failed:\n%s\n", info_log);
    }

    glDeleteShader(v_shader);
    glDeleteShader(f_shader);
    return program;
}

// ----------------------------------------------------------------------------------
// Internal Batch Flushing & Pipeline State
// ----------------------------------------------------------------------------------

static void flush_batch(void) {
    if (g_batch.vertex_count == 0) return;

    if (g_batch.mvp_dirty) {
        Matrix view_proj = matrix_multiply(g_batch.projection, g_batch.view);
        g_batch.mvp = matrix_multiply(view_proj, g_batch.model);
        g_batch.mvp_dirty = false;
    }

    glUseProgram(g_batch.shader_program);
    glUniformMatrix4fv(g_batch.mvp_loc, 1, GL_FALSE, g_batch.mvp.m);

    glBindVertexArray(g_batch.vao);
    glBindBuffer(GL_ARRAY_BUFFER, g_batch.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, g_batch.vertex_count * sizeof(Vertex), g_batch.vertices);

    GLenum gl_mode = (g_batch.current_draw_mode == MODE_TRIANGLES) ? GL_TRIANGLES : GL_LINES;
    glDrawArrays(gl_mode, 0, g_batch.vertex_count);

    glBindVertexArray(0);
    g_batch.vertex_count = 0;
}

static void set_draw_mode(DrawMode mode) {
    if (g_batch.current_draw_mode != mode) {
        flush_batch();
        g_batch.current_draw_mode = mode;
    }
}

static void add_vertex(Vector3 pos, Color color) {
    if (g_batch.vertex_count >= MAX_BATCH_VERTICES - 1) {
        flush_batch();
    }

    Vector4 c = color_to_vector4(color);
    g_batch.vertices[g_batch.vertex_count++] = (Vertex){
        pos.x, pos.y, pos.z,
        c.x, c.y, c.z, c.w
    };
}

static void recalculate_matrices(void) {
    flush_batch();

    int screen_width = get_screen_width();
    int screen_height = get_screen_height();

    if (g_batch.render_mode_2d_3d == 0) { // Default 2D screen space (0,0 top-left to width,height bottom-right)
        g_batch.projection = matrix_ortho(0.0f, (float)screen_width, (float)screen_height, 0.0f, -1.0f, 1.0f);
        g_batch.view = matrix_identity();
        g_batch.model = matrix_identity();
    } else if (g_batch.render_mode_2d_3d == 1) { // 2D Camera
        g_batch.projection = matrix_ortho(0.0f, (float)screen_width, (float)screen_height, 0.0f, -1000.0f, 1000.0f);
        g_batch.view = get_camera_matrix_2d(g_batch.camera_2d, screen_width, screen_height);
        g_batch.model = matrix_identity();
    } else if (g_batch.render_mode_2d_3d == 2) { // 3D Camera
        float aspect = (screen_height > 0) ? ((float)screen_width / (float)screen_height) : 1.0f;
        if (g_batch.camera_3d.projection == CAMERA_PERSPECTIVE) {
            g_batch.projection = matrix_perspective(g_batch.camera_3d.fovy * DEG2RAD, aspect, 0.1f, 1000.0f);
        } else {
            float top = g_batch.camera_3d.fovy * 0.5f;
            float right = top * aspect;
            g_batch.projection = matrix_ortho(-right, right, -top, top, 0.1f, 1000.0f);
        }
        g_batch.view = get_camera_matrix_3d(g_batch.camera_3d);
        g_batch.model = matrix_identity();
    }

    g_batch.mvp_dirty = true;
}

// ----------------------------------------------------------------------------------
// Internal Hooks for Init / Main Loop
// ----------------------------------------------------------------------------------

void internal_renderer_begin_frame(int width, int height) {
    (void)width; (void)height;
    if (g_batch.shader_program == 0) {
        g_batch.shader_program = create_program(vs_source, fs_source);
        g_batch.mvp_loc = glGetUniformLocation(g_batch.shader_program, "u_MVP");

        glGenVertexArrays(1, &g_batch.vao);
        glBindVertexArray(g_batch.vao);

        glGenBuffers(1, &g_batch.vbo);
        glBindBuffer(GL_ARRAY_BUFFER, g_batch.vbo);
        glBufferData(GL_ARRAY_BUFFER, MAX_BATCH_VERTICES * sizeof(Vertex), NULL, GL_DYNAMIC_DRAW);

        // Position attribute (0)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        // Color attribute (1)
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
}

// ----------------------------------------------------------------------------------
// Core Drawing Control Functions
// ----------------------------------------------------------------------------------

void begin_drawing(void) {
    internal_window_poll_events();

    g_batch.render_mode_2d_3d = 0;
    recalculate_matrices();
    glDisable(GL_DEPTH_TEST);
}

void end_drawing(void) {
    flush_batch();
    internal_window_swap_buffers();
}

void clear_background(Color color) {
    Vector4 c = color_to_vector4(color);
    glClearColor(c.x, c.y, c.z, c.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void begin_mode_2d(Camera2D camera) {
    flush_batch();
    g_batch.render_mode_2d_3d = 1;
    g_batch.camera_2d = camera;
    glDisable(GL_DEPTH_TEST);
    recalculate_matrices();
}

void end_mode_2d(void) {
    flush_batch();
    g_batch.render_mode_2d_3d = 0;
    glDisable(GL_DEPTH_TEST);
    recalculate_matrices();
}

void begin_mode_3d(Camera3D camera) {
    flush_batch();
    g_batch.render_mode_2d_3d = 2;
    g_batch.camera_3d = camera;
    glEnable(GL_DEPTH_TEST);
    recalculate_matrices();
}

void end_mode_3d(void) {
    flush_batch();
    g_batch.render_mode_2d_3d = 0;
    glDisable(GL_DEPTH_TEST);
    recalculate_matrices();
}

// ----------------------------------------------------------------------------------
// 2D Primitive Drawing Functions
// ----------------------------------------------------------------------------------

void draw_pixel(int x, int y, Color color) {
    draw_rectangle(x, y, 1, 1, color);
}

void draw_line(int start_x, int start_y, int end_x, int end_y, Color color) {
    set_draw_mode(MODE_LINES);
    add_vertex((Vector3){ (float)start_x, (float)start_y, 0.0f }, color);
    add_vertex((Vector3){ (float)end_x, (float)end_y, 0.0f }, color);
}

void draw_triangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color) {
    set_draw_mode(MODE_TRIANGLES);
    add_vertex((Vector3){ v1.x, v1.y, 0.0f }, color);
    add_vertex((Vector3){ v2.x, v2.y, 0.0f }, color);
    add_vertex((Vector3){ v3.x, v3.y, 0.0f }, color);
}

void draw_rectangle(int x, int y, int width, int height, Color color) {
    set_draw_mode(MODE_TRIANGLES);
    float fx = (float)x, fy = (float)y, fw = (float)width, fh = (float)height;

    add_vertex((Vector3){ fx, fy, 0.0f }, color);
    add_vertex((Vector3){ fx, fy + fh, 0.0f }, color);
    add_vertex((Vector3){ fx + fw, fy + fh, 0.0f }, color);

    add_vertex((Vector3){ fx, fy, 0.0f }, color);
    add_vertex((Vector3){ fx + fw, fy + fh, 0.0f }, color);
    add_vertex((Vector3){ fx + fw, fy, 0.0f }, color);
}

void draw_rectangle_lines(int x, int y, int width, int height, Color color) {
    set_draw_mode(MODE_LINES);
    float fx = (float)x, fy = (float)y, fw = (float)width, fh = (float)height;

    add_vertex((Vector3){ fx, fy, 0.0f }, color);
    add_vertex((Vector3){ fx + fw, fy, 0.0f }, color);

    add_vertex((Vector3){ fx + fw, fy, 0.0f }, color);
    add_vertex((Vector3){ fx + fw, fy + fh, 0.0f }, color);

    add_vertex((Vector3){ fx + fw, fy + fh, 0.0f }, color);
    add_vertex((Vector3){ fx, fy + fh, 0.0f }, color);

    add_vertex((Vector3){ fx, fy + fh, 0.0f }, color);
    add_vertex((Vector3){ fx, fy, 0.0f }, color);
}

void draw_circle(int center_x, int center_y, float radius, Color color) {
    set_draw_mode(MODE_TRIANGLES);
    int segments = 32;
    float step = (2.0f * PI) / segments;

    for (int i = 0; i < segments; i++) {
        float a1 = i * step;
        float a2 = (i + 1) * step;

        add_vertex((Vector3){ (float)center_x, (float)center_y, 0.0f }, color);
        add_vertex((Vector3){ center_x + cosf(a1) * radius, center_y + sinf(a1) * radius, 0.0f }, color);
        add_vertex((Vector3){ center_x + cosf(a2) * radius, center_y + sinf(a2) * radius, 0.0f }, color);
    }
}

void draw_circle_lines(int center_x, int center_y, float radius, Color color) {
    set_draw_mode(MODE_LINES);
    int segments = 32;
    float step = (2.0f * PI) / segments;

    for (int i = 0; i < segments; i++) {
        float a1 = i * step;
        float a2 = (i + 1) * step;

        add_vertex((Vector3){ center_x + cosf(a1) * radius, center_y + sinf(a1) * radius, 0.0f }, color);
        add_vertex((Vector3){ center_x + cosf(a2) * radius, center_y + sinf(a2) * radius, 0.0f }, color);
    }
}

void draw_poly(Vector2 center, int sides, float radius, float rotation, Color color) {
    if (sides < 3) return;
    set_draw_mode(MODE_TRIANGLES);
    float step = (2.0f * PI) / sides;
    float rot_rad = rotation * DEG2RAD;

    for (int i = 0; i < sides; i++) {
        float a1 = rot_rad + i * step;
        float a2 = rot_rad + (i + 1) * step;

        add_vertex((Vector3){ center.x, center.y, 0.0f }, color);
        add_vertex((Vector3){ center.x + cosf(a1) * radius, center.y + sinf(a1) * radius, 0.0f }, color);
        add_vertex((Vector3){ center.x + cosf(a2) * radius, center.y + sinf(a2) * radius, 0.0f }, color);
    }
}

void draw_text(const char *text, int pos_x, int pos_y, int font_size, Color color) {
    if (!text) return;
    int scale = (font_size > 8) ? (font_size / 8) : 1;
    if (scale < 1) scale = 1;

    int cur_x = pos_x;
    int cur_y = pos_y;

    for (const char *p = text; *p; p++) {
        char c = *p;
        if (c == '\n') {
            cur_y += 10 * scale;
            cur_x = pos_x;
            continue;
        }

        if (c < 32 || c > 126) c = '?';
        int glyph_index = c - 32;

        for (int row = 0; row < 8; row++) {
            unsigned char b = font8x8_basic[glyph_index][row];
            for (int col = 0; col < 8; col++) {
                if (b & (1 << col)) {
                    draw_rectangle(cur_x + col * scale, cur_y + row * scale, scale, scale, color);
                }
            }
        }
        cur_x += 9 * scale;
    }
}

// ----------------------------------------------------------------------------------
// 3D Primitive Drawing Functions
// ----------------------------------------------------------------------------------

void draw_line_3d(Vector3 start_pos, Vector3 end_pos, Color color) {
    set_draw_mode(MODE_LINES);
    add_vertex(start_pos, color);
    add_vertex(end_pos, color);
}

void draw_point_3d(Vector3 position, Color color) {
    draw_cube(position, 0.05f, 0.05f, 0.05f, color);
}

void draw_triangle_3d(Vector3 v1, Vector3 v2, Vector3 v3, Color color) {
    set_draw_mode(MODE_TRIANGLES);
    add_vertex(v1, color);
    add_vertex(v2, color);
    add_vertex(v3, color);
}

void draw_cube(Vector3 position, float width, float height, float length, Color color) {
    set_draw_mode(MODE_TRIANGLES);
    float x = position.x, y = position.y, z = position.z;
    float w = width * 0.5f, h = height * 0.5f, l = length * 0.5f;

    // 6 faces (12 triangles)
    // Front face (z + l)
    add_vertex((Vector3){ x - w, y - h, z + l }, color);
    add_vertex((Vector3){ x + w, y - h, z + l }, color);
    add_vertex((Vector3){ x + w, y + h, z + l }, color);
    add_vertex((Vector3){ x - w, y - h, z + l }, color);
    add_vertex((Vector3){ x + w, y + h, z + l }, color);
    add_vertex((Vector3){ x - w, y + h, z + l }, color);

    // Back face (z - l)
    add_vertex((Vector3){ x + w, y - h, z - l }, color);
    add_vertex((Vector3){ x - w, y - h, z - l }, color);
    add_vertex((Vector3){ x - w, y + h, z - l }, color);
    add_vertex((Vector3){ x + w, y - h, z - l }, color);
    add_vertex((Vector3){ x - w, y + h, z - l }, color);
    add_vertex((Vector3){ x + w, y + h, z - l }, color);

    // Top face (y + h)
    add_vertex((Vector3){ x - w, y + h, z + l }, color);
    add_vertex((Vector3){ x + w, y + h, z + l }, color);
    add_vertex((Vector3){ x + w, y + h, z - l }, color);
    add_vertex((Vector3){ x - w, y + h, z + l }, color);
    add_vertex((Vector3){ x + w, y + h, z - l }, color);
    add_vertex((Vector3){ x - w, y + h, z - l }, color);

    // Bottom face (y - h)
    add_vertex((Vector3){ x - w, y - h, z - l }, color);
    add_vertex((Vector3){ x + w, y - h, z - l }, color);
    add_vertex((Vector3){ x + w, y - h, z + l }, color);
    add_vertex((Vector3){ x - w, y - h, z - l }, color);
    add_vertex((Vector3){ x + w, y - h, z + l }, color);
    add_vertex((Vector3){ x - w, y - h, z + l }, color);

    // Right face (x + w)
    add_vertex((Vector3){ x + w, y - h, z + l }, color);
    add_vertex((Vector3){ x + w, y - h, z - l }, color);
    add_vertex((Vector3){ x + w, y + h, z - l }, color);
    add_vertex((Vector3){ x + w, y - h, z + l }, color);
    add_vertex((Vector3){ x + w, y + h, z - l }, color);
    add_vertex((Vector3){ x + w, y + h, z + l }, color);

    // Left face (x - w)
    add_vertex((Vector3){ x - w, y - h, z - l }, color);
    add_vertex((Vector3){ x - w, y - h, z + l }, color);
    add_vertex((Vector3){ x - w, y + h, z + l }, color);
    add_vertex((Vector3){ x - w, y - h, z - l }, color);
    add_vertex((Vector3){ x - w, y + h, z + l }, color);
    add_vertex((Vector3){ x - w, y + h, z - l }, color);
}

void draw_cube_wires(Vector3 position, float width, float height, float length, Color color) {
    set_draw_mode(MODE_LINES);
    float x = position.x, y = position.y, z = position.z;
    float w = width * 0.5f, h = height * 0.5f, l = length * 0.5f;

    // 12 edges
    Vector3 v[8] = {
        { x - w, y - h, z - l }, { x + w, y - h, z - l },
        { x + w, y + h, z - l }, { x - w, y + h, z - l },
        { x - w, y - h, z + l }, { x + w, y - h, z + l },
        { x + w, y + h, z + l }, { x - w, y + h, z + l }
    };

    // Front/Back rectangles
    for (int i = 0; i < 4; i++) {
        draw_line_3d(v[i], v[(i + 1) % 4], color);
        draw_line_3d(v[4 + i], v[4 + ((i + 1) % 4)], color);
        draw_line_3d(v[i], v[i + 4], color);
    }
}

void draw_sphere_wires(Vector3 center_pos, float radius, int rings, int slices, Color color) {
    if (rings < 3) rings = 3;
    if (slices < 3) slices = 3;

    set_draw_mode(MODE_LINES);
    for (int i = 0; i <= rings; i++) {
        float phi = PI * (float)i / rings;
        for (int j = 0; j < slices; j++) {
            float theta1 = 2.0f * PI * (float)j / slices;
            float theta2 = 2.0f * PI * (float)(j + 1) / slices;

            Vector3 p1 = {
                center_pos.x + radius * sinf(phi) * cosf(theta1),
                center_pos.y + radius * cosf(phi),
                center_pos.z + radius * sinf(phi) * sinf(theta1)
            };
            Vector3 p2 = {
                center_pos.x + radius * sinf(phi) * cosf(theta2),
                center_pos.y + radius * cosf(phi),
                center_pos.z + radius * sinf(phi) * sinf(theta2)
            };
            draw_line_3d(p1, p2, color);
        }
    }
}

void draw_grid(int slices, float spacing) {
    int half_slices = slices / 2;
    set_draw_mode(MODE_LINES);

    for (int i = -half_slices; i <= half_slices; i++) {
        Color c = (i == 0) ? (Color){ 200, 50, 50, 255 } : LIGHTGRAY;
        draw_line_3d((Vector3){ (float)i * spacing, 0.0f, (float)-half_slices * spacing },
                     (Vector3){ (float)i * spacing, 0.0f, (float)half_slices * spacing }, c);

        c = (i == 0) ? (Color){ 50, 50, 200, 255 } : LIGHTGRAY;
        draw_line_3d((Vector3){ (float)-half_slices * spacing, 0.0f, (float)i * spacing },
                     (Vector3){ (float)half_slices * spacing, 0.0f, (float)i * spacing }, c);
    }
}
