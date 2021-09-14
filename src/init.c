#include "init.h"

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


static void init_texture(GLuint* tex, int tex_unit, const char* tex_path)
{
    // Activate the first texture unit:
    glActiveTexture(tex_unit);
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    gl_check_error("glTexParameteri [min_filter]");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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

    // Associate the sampler "tex" with texture unit 0:
    user_data->background_sampler_uniform = glGetUniformLocation(user_data->shader_program_back, "background");
    gl_check_error("glGetUniformLocation [background_sampler_uniform]");

    user_data->digit_pos_uniform = glGetUniformLocation(user_data->shader_program_font, "pos");
    user_data->digit_tex_uniform = glGetUniformLocation(user_data->shader_program_font, "digit");
    gl_check_error("glGetUniformLocation [digit_...]");

    user_data->block_pos_uniform   = glGetUniformLocation(user_data->shader_program_single_block, "pos");
    user_data->block_id_uniform    = glGetUniformLocation(user_data->shader_program_single_block, "block_id");
    user_data->block_scale_uniform = glGetUniformLocation(user_data->shader_program_single_block, "scale");
    gl_check_error("glGetUniformLocation [block_...]");
}


static void load_model(const char* model_path, GLuint vao, GLuint vbo, int* vertex_data_count)
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

    user_data->holding_left = false;
    user_data->holding_right = false;
    user_data->time_since_last_side_move = 0.0;
}

void load_audio_files(struct WavData** data)
{
    data[0] = load_wav_file(TETRIS_BACKGROUND_MUSIC);
    data[1] = load_wav_file(TETRIS_DEFEAT_SOUND);
    data[2] = load_wav_file(TETRIS_FOR_JEFF);
}

void init_gl(GLFWwindow* window)
{
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // load the soundtracks
    user_data->wav_data = calloc(sizeof(struct WavData*), NUMBER_OF_AUDIO_FILES);
    load_audio_files(user_data->wav_data);
    SDL_AudioDeviceID deviceId = open_audio_device(user_data->wav_data[0]);
    SDL_AudioDeviceID deviceId2 = open_audio_device(user_data->wav_data[1]);

    user_data->background_device = deviceId;
    user_data->effect_device = deviceId2;

    // Initialize our shader programs:
    create_shader_program(
        &user_data->shader_program_blocks,
        compile_shader(GL_VERTEX_SHADER, "shader/vertex_blocks.glsl", "Vertex Blocks Shader"),
        compile_shader(GL_FRAGMENT_SHADER, "shader/fragment_blocks.glsl", "Fragment Blocks Shader")
    );

    create_shader_program(
        &user_data->shader_program_arena,
        compile_shader(GL_VERTEX_SHADER, "shader/vertex_arena.glsl", "Vertex Arena Shader"),
        compile_shader(GL_FRAGMENT_SHADER, "shader/fragment_arena.glsl", "Fragment Arena Shader")
    );

    create_shader_program(
        &user_data->shader_program_back,
        compile_shader(GL_VERTEX_SHADER, "shader/vertex_back.glsl", "Vertex Backs Shader"),
        compile_shader(GL_FRAGMENT_SHADER, "shader/fragment_back.glsl", "Fragment Backs Shader")
    );

    create_shader_program(
        &user_data->shader_program_font,
        compile_shader(GL_VERTEX_SHADER, "shader/vertex_font.glsl", "Vertex Font Shader"),
        compile_shader(GL_FRAGMENT_SHADER, "shader/fragment_font.glsl", "Fragment Font Shader")
    );

    create_shader_program(
        &user_data->shader_program_single_block,
        compile_shader(GL_VERTEX_SHADER, "shader/vertex_block.glsl", "Vertex Block Shader"),
        compile_shader(GL_FRAGMENT_SHADER, "shader/fragment_block.glsl", "Fragment Block Shader")
    );

    // Initialize our texture:
    glGenTextures(11, user_data->textures);
    init_texture(user_data->textures, GL_TEXTURE0, TEX_BACKGROUND_GRID);

    char file_path[] = "textures/_.bmp";

    for (size_t i = 0; i < 10; i++) {
        sprintf(file_path, "textures/%ld.bmp", i);

        GLuint texunit;
        switch (i)
        {
            case 0:
                texunit = GL_TEXTURE1;
                break;
            case 1:
                texunit = GL_TEXTURE2;
                break;
            case 2:
                texunit = GL_TEXTURE3;
                break;
            case 3:
                texunit = GL_TEXTURE4;
                break;
            case 4:
                texunit = GL_TEXTURE5;
                break;
            case 5:
                texunit = GL_TEXTURE6;
                break;
            case 6:
                texunit = GL_TEXTURE7;
                break;
            case 7:
                texunit = GL_TEXTURE8;
                break;
            case 8:
                texunit = GL_TEXTURE9;
                break;
            case 9:
                texunit = GL_TEXTURE10;
        }

        init_texture(user_data->textures + i + 1, texunit, file_path);
    }

    // Initialize our model:
    init_model(user_data);

    // Initialize our uniforms:
    init_uniforms(user_data);

    // Initialize our vertex data:
    // init_vertex_data(user_data);

    glGenVertexArrays(10, user_data->vao);
    glGenBuffers(10, user_data->vbo);
    load_model(MODEL_PATH_BLOCK,      user_data->vao[0], user_data->vbo[0], user_data->vertex_data_count + 0);
    load_model(MODEL_PATH_ARENA,      user_data->vao[1], user_data->vbo[1], user_data->vertex_data_count + 1);
    load_model(MODEL_PATH_BACKGROUND, user_data->vao[2], user_data->vbo[2], user_data->vertex_data_count + 2);
    load_model(MODEL_O,               user_data->vao[3], user_data->vbo[3], user_data->vertex_data_count + 3);
    load_model(MODEL_L,               user_data->vao[4], user_data->vbo[4], user_data->vertex_data_count + 4);
    load_model(MODEL_J,               user_data->vao[5], user_data->vbo[5], user_data->vertex_data_count + 5);
    load_model(MODEL_T,               user_data->vao[6], user_data->vbo[6], user_data->vertex_data_count + 6);
    load_model(MODEL_I,               user_data->vao[7], user_data->vbo[7], user_data->vertex_data_count + 7);
    load_model(MODEL_Z,               user_data->vao[8], user_data->vbo[8], user_data->vertex_data_count + 8);
    load_model(MODEL_S,               user_data->vao[9], user_data->vbo[9], user_data->vertex_data_count + 9);


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

void free_audio_files(struct WavData** data)
{
    for (size_t i = 0; i < NUMBER_OF_AUDIO_FILES; i++) {
        free_wav_file(data[i]);
    }
}

void teardown_gl(GLFWwindow* window)
{
    printf("Tearing down ...\n");
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // Free the soundtracks again and close the audio devices
    close_audio_device(user_data->background_device);
    close_audio_device(user_data->effect_device);
    free_audio_files(user_data->wav_data);

    // Delete the shader programs:
    glDeleteProgram(user_data->shader_program_blocks);
    glDeleteProgram(user_data->shader_program_back);
    glDeleteProgram(user_data->shader_program_arena);
    glDeleteProgram(user_data->shader_program_font);
    glDeleteProgram(user_data->shader_program_single_block);
    gl_check_error("glDeleteProgram");

    // Delete the VAO:
    glDeleteVertexArrays(10, user_data->vao);
    gl_check_error("glDeleteVertexArrays");

    // Delete the VBO:
    glDeleteBuffers(10, user_data->vbo);
    gl_check_error("glDeleteBuffers");

    // Delete the texture:
    glDeleteTextures(11, user_data->textures);
    gl_check_error("glDeleteTextures");
}