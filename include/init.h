#ifndef INIT_H_
#define INIT_H_


#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "audio.h"
#include "bitmap.h"
#include "engine.h"
#include "error.h"
#include "obj.h"
#include "user_data.h"

// model paths
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

// sound paths
#define TETRIS_BACKGROUND_MUSIC "sfx/tetris_bg_music.wav"
#define TETRIS_DEFEAT_SOUND "sfx/defeat.wav"
#define TETRIS_FOR_JEFF "sfx/tetris_for_jeff.wav"
#define TETRIS_SIO_SOUND "sfx/tetris_sio_sound.wav"

// texture paths
#define TEX_BACKGROUND_GRID "textures/grid.bmp"
#define TEX_DIGIT_FORMAT_PATH "textures/%ld.bmp"
#define TEX_KEYMAP "textures/keymap.bmp"
#define TEX_GAMEOVER "textures/gameover.bmp"
#define TEX_CLEARED_LINES "textures/ClearedLines.bmp"
#define TEX_LEVEL "textures/Level.bmp"
#define TEX_SCORE "textures/Score.bmp"

// texture unit locations
#define TEX_LOC_BACKGROUND 0
#define TEX_LOC_KEYMAP 11
#define TEX_LOC_GAMEOVER 12
#define TEX_LOC_CLEARED_LINES 13
#define TEX_LOC_LEVEL 14
#define TEX_LOC_SCORE 15

// vertex attributes positions
#define ATTRIB_POSITION 0
#define ATTRIB_COLOR 1
#define ATTRIB_NORMAL 2
#define ATTRIB_TEX_COORDS 3

#define NUMBER_OF_AUDIO_FILES 4

void init_gl(GLFWwindow* window);
void teardown_gl(GLFWwindow* window);

#endif