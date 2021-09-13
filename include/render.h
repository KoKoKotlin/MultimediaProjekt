#ifndef GLCALLS_H
#define GLCALLS_H

// Include the GLAD loader *before* including GLFW!
#include "glad/glad.h"

// Include the GLFW library (should be the same for all OS):
#include <GLFW/glfw3.h>

#include "engine.h"
#include "audio.h"

typedef struct
{
    // Dimensions of the window:
    int window_width;
    int window_height;

    // The current shader programs:
    GLuint shader_program_arena;
    GLuint shader_program_back;
    GLuint shader_program_blocks;
    GLuint shader_program_font;
    GLuint shader_program_single_block;

    // The vertex array object (blackbox ...):
    GLuint vao[10];

    // The vertex buffer object (GPU mem for the vertex data):
    GLuint vbo[10];

    // The number of vertices to render:
    int vertex_data_count[10];

    GLuint textures[1];

    // The model:
    double last_frame_time;
    double time_since_last_drop;
    struct GameData gameData;

    // uniform for instanced rendering
    GLint block_positions;
    GLint background_sampler_uniform;
    GLint digit_pos_uniform;
    GLint digit_tex_uniform;
    GLint block_pos_uniform;
    GLint block_id_uniform;

    // for sound
    SDL_AudioDeviceID background_device;
    SDL_AudioDeviceID effect_device;
    struct WavData** wav_data;

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