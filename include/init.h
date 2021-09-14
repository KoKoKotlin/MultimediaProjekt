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

#define NUMBER_OF_AUDIO_FILES 3

void init_gl(GLFWwindow* window);
void teardown_gl(GLFWwindow* window);



#endif