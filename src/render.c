#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SDL2/SDL.h>

#include "render.h"
#include "bitmap.h"
#include "obj.h"
#include "engine.h"

#define MODEL_PATH_BLOCK "models/Block_basic.obj"
#define MODEL_PATH_ARENA "models/arena.obj"
#define MODEL_PATH_BACKGROUND "models/background_arena.obj"

#define TEX_BACKGROUND_GRID "textures/grid.bmp"

// #define TEX_PATH   "models/logo.bmp"
#define Y_ANGULAR_VELOCITY 2

static char* read_shader_source_from_file(const char* path)
{
    // Open the file:
    FILE* file = fopen(path , "rb");
    check_error(file != NULL, "Failed to open file.");

    // Seek to the end:
    int success = fseek(file, 0, SEEK_END);
    check_error(success == 0, "Failed to forward file pointer.");

    // Obtain the size of the file from the position of the file pointer:
    long size = ftell(file);
    check_error(size >= 0, "Failed to determine file size.");

    // Rewind the file pointer back to the start:
    rewind(file);

    // Allocate the output buffer:
    char* buffer = malloc(size + 1);
    check_error(buffer != NULL, "Failed to allocate buffer.");

    // Read into it:
    size_t blocks_read = fread(buffer, size, 1, file);
    check_error(blocks_read == 1, "Failed to read file.");

    // Close the file:
    success = fclose(file);
    check_error(success == 0, "Failed to close file.");

    // Append a null-terminator and return the buffer:
    buffer[size] = '\0';

    return buffer;
}

static GLuint compile_shader(GLenum type, const char* shader_path, const char* shader_tag)
{
    // Create an empty shader:
    GLuint shader = glCreateShader(type);
    gl_check_error("glCreateShader");

    // Read and specify the source code:
    char* shader_source = read_shader_source_from_file(shader_path);

    glShaderSource(shader, 1, (const char**)&shader_source, NULL);
    gl_check_error("glShaderSource");

    free(shader_source);

    // Compile the shader:
    glCompileShader(shader);
    gl_check_error("glCompileShader");

    // Check the compilation status:
    GLint success;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    gl_check_error("glGetShaderiv");

    if (success)
    {
        return shader;
    }

    // Extract the length of the error message (including '\0'):
    GLint info_length = 0;

    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
    gl_check_error("glGetShaderiv");

    if (info_length > 1)
    {
        // Extract the message itself:
        char* info = malloc(info_length);
        check_error(info != NULL, "Failed to allocate memory for error message.");

        glGetShaderInfoLog(shader, info_length, NULL, info);
        gl_check_error("glGetShaderInfoLog");

        fprintf(stderr, "Error compiling shader (%s): %s\n", shader_tag, info);
        free(info);
    }
    else
    {
        fprintf(stderr, "No info log from the shader compiler :(\n");
    }

    exit(EXIT_FAILURE);
}

static void create_shader_program(GLuint* shader_handle, GLuint vertex_shader, GLuint fragment_shader)
{
    // Create an empty shader program:
    printf("Creating shader program ...\n");

    GLuint shader_program = glCreateProgram();
    gl_check_error("glCreateProgram");

    // Attach both shaders to the program:
    glAttachShader(shader_program, vertex_shader);
    gl_check_error("glAttachShader [vertex]");

    glAttachShader(shader_program, fragment_shader);
    gl_check_error("glAttachShader [fragment]");

    // Link the shader program:
    glLinkProgram(shader_program);
    gl_check_error("glLinkProgram");

    // Detach the shaders after linking:
    glDetachShader(shader_program, vertex_shader);
    gl_check_error("glDetachShader [vertex]");

    glDetachShader(shader_program, fragment_shader);
    gl_check_error("glDetachShader [fragment]");

    // Delete the shaders:
    glDeleteShader(vertex_shader);
    gl_check_error("glDeleteShader [vertex]");

    glDeleteShader(fragment_shader);
    gl_check_error("glDeleteShader [fragment]");

    // Check the link status:
    GLint success;

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    gl_check_error("glGetProgramiv");

    if (success)
    {
        // Use the program from now on:
        gl_check_error("glUseProgram");

        // We can now release the shader compiler.
        glReleaseShaderCompiler();
        gl_check_error("glReleaseShaderCompiler");

        *shader_handle = shader_program;
        return;
    }

    // Extract the length of the error message (including '\0'):
    GLint info_length = 0;
    glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &info_length);
    gl_check_error("glGetProgramiv");

    if (info_length > 1)
    {
        // Extract the message itself:
        char* info = malloc(info_length);
        check_error(info != NULL, "Failed to allocate memory for error message.");

        glGetProgramInfoLog(shader_program, info_length, NULL, info);
        gl_check_error("glGetProgramInfoLog");

        fprintf(stderr, "Error linking shader program: %s\n", info);
        free(info);
    }
    else
    {
        fprintf(stderr, "No info log from the shader compiler :(\n");
    }

    exit(EXIT_FAILURE);
}


static void init_texture(GLuint* tex, const char* tex_path)
{
    // Activate the first texture unit:
    glActiveTexture(GL_TEXTURE0);
    gl_check_error("glActiveTexture");

    // Bind it to the 2D binding point *of texture unit 0*:
    glBindTexture(GL_TEXTURE_2D, *tex);
    gl_check_error("glBindTexture");

    // Specify wrapping (uv <=> st):
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    gl_check_error("glTexParameteri [wrap_u]");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    gl_check_error("glTexParameteri [wrap_v]");

    // Specify filtering:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    gl_check_error("glTexParameteri [min_filter]");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gl_check_error("glTexParameteri [mag_filter]");

    // Load the bitmap:
    bitmap_pixel_rgb_t* pixels;
    uint32_t width, height;

    bitmap_error_t err = bitmapReadPixels(tex_path, (bitmap_pixel_t**)&pixels, &width, &height, BITMAP_COLOR_SPACE_RGB);
    check_error(err == BITMAP_ERROR_SUCCESS, "Failed to load texture bitmap.");

    // Upload the texture pixels to the GPU:
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    gl_check_error("glTexImage2D");

    free(pixels);
}



static void init_uniforms(user_data_t* user_data)
{
    user_data->block_positions = glGetUniformLocation(user_data->shader_program_blocks, "block_positions");
    gl_check_error("glGetUniformLocation [block_position]");

    user_data->background_sampler_uniform = glGetUniformLocation(user_data->shader_program_back, "tex");

    // Associate the sampler "tex" with texture unit 0:

    gl_check_error("glUniform1iv [block_positions]");
}


static void load_model(char* model_path, GLuint vao, GLuint vbo, int* vertex_data_count)
{
    glBindVertexArray(vao);
    gl_check_error("glBindVertexArray");

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    gl_check_error("glBindBuffer");

    // Open the obj file:
    FILE* obj = fopen(model_path, "r");

    //check for load error
    char* error_msg_start = "Failed to open obj file at: ";
    char* error_msg = calloc(sizeof(char), strlen(error_msg_start) + strlen(model_path) + 1);
    strcat(error_msg, error_msg_start);
    strcat(error_msg, model_path);
    check_error(obj != NULL, error_msg);

    // Count the entries:
    int vertex_count = 0;
    int tex_coords_count = 0;
    int normal_count = 0;
    int face_count = 0;
    int mtl_lib_count = 0;

    obj_count_entries(obj, &vertex_count, &tex_coords_count, &normal_count, &face_count, &mtl_lib_count);

    printf("Parsed obj file \"%s\":\n", MODEL_PATH_BLOCK);
    printf("\tVertices: %d\n", vertex_count);
    printf("\tTexture coordinates: %d\n", tex_coords_count);
    printf("\tNormals: %d\n", normal_count);
    printf("\tFaces: %d\n", face_count);
    printf("\tMaterial libraries: %d\n", mtl_lib_count);

    // Rewind the file pointer:
    rewind(obj);

    // - one array for the obj vertices
    obj_vertex_entry_t *vertices = malloc(sizeof(obj_vertex_entry_t) * vertex_count);
    check_error(vertices != NULL, "Failed to allocate memory for vertices.");

    // - one array for the obj tex coords
    obj_tex_coords_entry_t *tex_coords = malloc(sizeof(obj_tex_coords_entry_t) * tex_coords_count);
    check_error(tex_coords != NULL, "Failed to allocate memory for tex coordinates.");

    // - one array for the obj normals
    obj_normal_entry_t *normals = malloc(sizeof(obj_normal_entry_t) * normal_count);
    check_error(normals != NULL, "Failed to allocate memory for normals.");

    // - one array for the vertex data structs (built from the faces)
    vertex_data_t *vertex_data = malloc(sizeof(vertex_data_t) * face_count * 3);
    check_error(vertex_data != NULL, "Failed to allocate memory for vertex data.");

    obj_entry_type_t entry_type;
    obj_entry_t entry;

    size_t vertex_index = 0;
    size_t tex_coord_index = 0;
    size_t normal_index = 0;
    size_t vertex_data_index = 0;

    // TODO: Clean index validation!
    while ((entry_type = obj_get_next_entry(obj, &entry)) != OBJ_ENTRY_TYPE_END)
    {
        switch (entry_type)
        {
            case OBJ_ENTRY_TYPE_VERTEX: vertices[vertex_index++] = entry.vertex_entry; break;
            case OBJ_ENTRY_TYPE_TEX_COORDS: tex_coords[tex_coord_index++] = entry.tex_coords_entry; break;
            case OBJ_ENTRY_TYPE_NORMAL: normals[normal_index++] = entry.normal_entry; break;

            case OBJ_ENTRY_TYPE_FACE:

                for (size_t i = 0; i < 3; i++)
                {
                    const obj_vertex_entry_t* vertex = &vertices[entry.face_entry.triples[i].vertex_index];
                    const obj_normal_entry_t* normal = &normals[entry.face_entry.triples[i].normal_index];
                    const obj_tex_coords_entry_t* tex_coord = &tex_coords[entry.face_entry.triples[i].tex_coords_index];

                    vertex_data[vertex_data_index++] = (vertex_data_t)
                    {
                        .position = { vertex->x, vertex->y, vertex->z },
                        .color = { 0xFF, 0xFF, 0xFF },
                        .normal = { normal->x, normal->y, normal->z },
                        .tex_coords = { tex_coord->u, tex_coord->v }
                    };
                }

                break;

            default: break;
        }
    }
    *vertex_data_count = face_count * 3;

    // Upload the data to the GPU:
    glBufferData(GL_ARRAY_BUFFER, *vertex_data_count * sizeof(vertex_data_t), (const GLvoid*)vertex_data, GL_STATIC_DRAW);

    fclose(obj);
    free(vertices);
    free(tex_coords);
    free(normals);
    free(vertex_data);

    // Position attribute:
    // Number of attribute, number of components, data type, normalize, stride, pointer (= offset)
    glVertexAttribPointer(ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, position));
    gl_check_error("glVertexAttribPointer [position]");

    glVertexAttribPointer(ATTRIB_COLOR, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, color));
    gl_check_error("glVertexAttribPointer [color]");

    glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, normal));
    gl_check_error("glVertexAttribPointer [normal]");

    glVertexAttribPointer(ATTRIB_TEX_COORDS, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_data_t), (GLvoid*)offsetof(vertex_data_t, tex_coords));
    gl_check_error("glVertexAttribPointer [tex coords]");

    // Enable the attributes:
    glEnableVertexAttribArray(ATTRIB_POSITION);
    gl_check_error("glEnableVertexAttribArray [position]");

    glEnableVertexAttribArray(ATTRIB_COLOR);
    gl_check_error("glEnableVertexAttribArray [color]");

    glEnableVertexAttribArray(ATTRIB_NORMAL);
    gl_check_error("glEnableVertexAttribArray [normal]");

    glEnableVertexAttribArray(ATTRIB_TEX_COORDS);
    gl_check_error("glEnableVertexAttribArray [tex coords]");
}

static void init_model(user_data_t* user_data)
{
    user_data->last_frame_time = glfwGetTime();
    user_data->time_since_last_drop = 0.0;
    user_data->gameData = init_gamedata(0);
}

void check_error(int condition, const char* error_text)
{
    if (!condition)
    {
        fprintf(stderr, "%s\n", error_text);
        exit(EXIT_FAILURE);
    }
}

void gl_check_error(const char* error_text)
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        fprintf(stderr, "GLError: %s (%d)\n", error_text, error);
        exit(EXIT_FAILURE);
    }
}

void init_gl(GLFWwindow* window)
{
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // Initialize our shader programs:
    create_shader_program(
        &user_data->shader_program_blocks,
        compile_shader(GL_VERTEX_SHADER, "shader/vertex_blocks.glsl", "Vertex Blocks Shader"),
        compile_shader(GL_FRAGMENT_SHADER, "shader/fragment_blocks.glsl", "Fragment Blocks Shader")
    );

    create_shader_program(
        &user_data->shader_program_back,
        compile_shader(GL_VERTEX_SHADER, "shader/vertex_back.glsl", "Vertex Backs Shader"),
        compile_shader(GL_FRAGMENT_SHADER, "shader/fragment_back.glsl", "Fragment Backs Shader")
    );

    // Initialize our texture:
    glGenTextures(1, &user_data->textures[0]);
    init_texture(&user_data->textures[0], TEX_BACKGROUND_GRID);

    // Initialize our model:
    init_model(user_data);

    // Initialize our uniforms:
    init_uniforms(user_data);

    // Initialize our vertex data:
    // init_vertex_data(user_data);

    glGenVertexArrays(3, user_data->vao);
    glGenBuffers(3, user_data->vbo);
    load_model(MODEL_PATH_BLOCK,      user_data->vao[0], user_data->vbo[0], user_data->vertex_data_count + 0);
    load_model(MODEL_PATH_ARENA,      user_data->vao[1], user_data->vbo[1], user_data->vertex_data_count + 1);
    load_model(MODEL_PATH_BACKGROUND, user_data->vao[2], user_data->vbo[2], user_data->vertex_data_count + 2);

    // Obtain the internal size of the framebuffer:
    int fb_width, fb_height;
    glfwGetFramebufferSize(window, &fb_width, &fb_height);

    // Align the viewport to the framebuffer:
    glViewport(0, 0, fb_width, fb_height);
    gl_check_error("glViewport");

    // Specify the clear color:
    glClearColor(0.1, 0.1, 0.1, 1);
    gl_check_error("glClearColor");

    // Enable the depth test:
    glEnable(GL_DEPTH_TEST);
    gl_check_error("glEnable [depth test]");

    // Enable backface culling:
    glEnable(GL_CULL_FACE);
}

void handle_background_sound(user_data_t* user_data)
{
    if (SDL_GetQueuedAudioSize(user_data->background_device) == 0)
        queue_audio(user_data->background_device, user_data->wav_data[0]);
}

void update_gl(GLFWwindow* window)
{
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // Calculate the frame delta time and update the timestamp:
    double frame_time = glfwGetTime();
    double delta_time = frame_time - user_data->last_frame_time;
    if (user_data->gameData.gameState == PLAYING) {
        // accumulate delta time
        user_data->time_since_last_drop += delta_time;

        if ((!user_data->gameData.fast_drop && (user_data->time_since_last_drop >= calc_drop_time(&user_data->gameData))) ||
            (user_data->gameData.fast_drop && user_data->time_since_last_drop >= FAST_DROP_TIME)) {
            size_t cleared_rows = drop(&user_data->gameData);

            if (cleared_rows == 4) queue_audio(user_data->effect_device, user_data->wav_data[2]);
            user_data->time_since_last_drop = 0.0;

            if (user_data->gameData.is_defeat) {
                queue_audio(user_data->effect_device, user_data->wav_data[1]);
                user_data->gameData.gameState = GAME_OVER;
            }
        }
    }
    user_data->last_frame_time = frame_time;

    SDL_PauseAudioDevice(user_data->background_device, (user_data->gameData.gameState == PAUSE) ? 1 : 0);
    handle_background_sound(user_data);
}

void draw_gl(GLFWwindow* window)
{
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // Clear the color buffer -> background color:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_check_error("glClear");

    glUseProgram(user_data->shader_program_back);
    // FIXME draw background plane
    for (size_t i = 1; i < 3; i++) {
        glBindVertexArray(user_data->vao[i]);
        glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[i]);

        // TODO: texture on background
        if (i == 1) {

        } else if (i == 2) {

        }

        glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count[i]);
    }

    glBindVertexArray(user_data->vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[0]);

    int block_positions[200] = { 0 };
    generate_block_positions(&user_data->gameData, block_positions);

    glUseProgram(user_data->shader_program_blocks);
    glUniform1iv(user_data->block_positions, 200, block_positions);

    // Parameters: primitive type, start index, count
    glDrawArraysInstanced(GL_TRIANGLES, 0, user_data->vertex_data_count[0], 200);
    gl_check_error("glDrawArraysInstanced");
}

void teardown_gl(GLFWwindow* window)
{
    printf("Tearing down ...\n");
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // Delete the shader program:
    glDeleteProgram(user_data->shader_program_blocks);
    glDeleteProgram(user_data->shader_program_back);
    gl_check_error("glDeleteProgram");

    // Delete the VAO:
    glDeleteVertexArrays(3, user_data->vao);
    gl_check_error("glDeleteVertexArrays");

    // Delete the VBO:
    glDeleteBuffers(3, user_data->vbo);
    gl_check_error("glDeleteBuffers");

    // Delete the texture:
    glDeleteTextures(1, user_data->textures);
    gl_check_error("glDeleteTextures");
}