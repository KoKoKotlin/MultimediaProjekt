#ifndef USER_DATA_H_
#define USER_DATA_H_

#include <SDL2/SDL.h>
#include "engine.h"

#include "glad/glad.h"

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
    GLuint shader_program_image;

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
    double time_since_last_side_move;

    bool holding_right;
    bool holding_left;

    struct GameData gameData;

    // uniform for instanced rendering
    GLint block_positions;
    GLint background_sampler_uniform;
    GLint digit_pos_uniform;
    GLint digit_tex_uniform;
    GLint block_pos_uniform;
    GLint block_id_uniform;
    GLint block_scale_uniform;
    GLint image_pos_uniform;
    GLint image_scale_uniform;
    GLint image_tex_uniform;

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

#endif