#ifndef INIT_H_
#define INIT_H_

#include <SDL2/SDL.h>
#include <stdlib.h>
#include <stdio.h>

#include "audio.h"
#include "error.h"
#include "glad/glad.h"
#include "obj.h"

#define MODEL_PATH_BLOCK "models/Block_basic.obj"
#define MODEL_PATH_ARENA "models/arena.obj"
#define MODEL_PATH_BACKGROUND "models/background.obj"

#define MODEL_O "models/Block_O.obj"
#define MODEL_L "models/Block_L.obj"
#define MODEL_J "models/Block_J.obj"
#define MODEL_T "models/Block_T.obj"
#define MODEL_I "models/Block_I.obj"
#define MODEL_Z "models/Block_Z.obj"
#define MODEL_S "models/Block_S.obj"

#define TEX_BACKGROUND_GRID "textures/grid.bmp"
#define Y_ANGULAR_VELOCITY 2

// Vertex attributes:
#define ATTRIB_POSITION 0
#define ATTRIB_COLOR 1
#define ATTRIB_NORMAL 2
#define ATTRIB_TEX_COORDS 3


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

void init_gl(GLFWwindow* window)
void teardown_gl(GLFWwindow* window);

#endif