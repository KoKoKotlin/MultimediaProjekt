#ifndef GLCALLS_H
#define GLCALLS_H

// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>

#include "engine.h"

typedef struct
{
    // Dimensions of the window:
    int window_width;
    int window_height;

    // The current shader program:
    GLuint shader_program_blocks;
    GLuint shader_program_back;

    // The vertex array object (blackbox ...):
    GLuint vao[3];

    // The vertex buffer object (GPU mem for the vertex data):
    GLuint vbo[3];

    // The number of vertices to render:
    int vertex_data_count[3];

    // The model:
    double last_frame_time;
    double time_since_last_drop;
    struct GameData gameData;

    // unniform for instanced rendering
    GLuint block_positions;
} user_data_t;

typedef struct
{
    GLfloat position[3];
    GLubyte color[3];
    GLfloat normal[3];
    GLfloat tex_coords[2];
} vertex_data_t;

// Vertex attributes:
#define ATTRIB_POSITION 0
#define ATTRIB_COLOR 1
#define ATTRIB_NORMAL 2
#define ATTRIB_TEX_COORDS 3

// Generic error checks:
void check_error(int condition, const char* error_text);
void gl_check_error(const char* error_text);

// GL functions:
void init_gl(GLFWwindow* window);
void update_gl(GLFWwindow* window);
void draw_gl(GLFWwindow* window);
void teardown_gl(GLFWwindow* window);

#endif