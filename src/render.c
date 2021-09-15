#include "render.h"

void draw_string(const user_data_t* user_data, const char* string, double pos_x, double pos_y)
{
    double offset = 0.12;
    for (size_t i = 0; i < strlen(string); i++) {

        // set position and texture for next char
        GLfloat pos[] = { pos_x + offset * i, pos_y };
        glUseProgram(user_data->shader_program_font);

        glUniform1i(user_data->digit_tex_uniform, (GLint)(string[i] - '0' + 1));
        gl_check_error("glUniform1i Font");
        glUniform2fv(user_data->digit_pos_uniform, 1, pos);
        gl_check_error("glUniform2fv Font");

        // draw single char
        glBindVertexArray(user_data->vao[2]);
        glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[2]);
        glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count[2]);
        gl_check_error("glDrawArrays Font");
    }
}

void draw_image(const user_data_t* user_data, GLint texunit, GLfloat* pos, GLfloat* scale)
{
    glUseProgram(user_data->shader_program_image);

    glUniform1i(user_data->image_tex_uniform, texunit);
    gl_check_error("glUniform1i image");
    glUniform3fv(user_data->image_pos_uniform, 1, pos);
    gl_check_error("glUniform3fv image");
    glUniform2fv(user_data->image_scale_uniform, 1, scale);
    gl_check_error("glUniform2fv image");

    // draw plane with image texture
    glBindVertexArray(user_data->vao[2]);
    glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[2]);
    glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count[2]);
    gl_check_error("glDrawArrays image");
}

void draw_text(const user_data_t* user_data)
{
    // create text buffers
    char score[7];
    memset(score, 0, 7);
    char level[3];
    memset(level, 0, 3);
    char cleared_lines[7];
    memset(cleared_lines, 0, 7);

    // fill the buffers
    sprintf(score, "%06d", user_data->gameData.score);
    sprintf(level, "%02d", user_data->gameData.level);
    sprintf(cleared_lines, "%06d", user_data->gameData.cleared_lines);

    // draw the string data with the labels
    draw_string(user_data, score, 0.835, 0.1);
    GLfloat score_pos[]   = { 0.95, 0.23, 0.0 };
    GLfloat score_scale[] = { 2.5, 0.5 };
    draw_image(user_data, TEX_LOC_SCORE, score_pos, score_scale);

    draw_string(user_data, level, 1.1, -0.38);
    GLfloat level_pos[]   = { 0.95, -0.25, 0.0 };
    GLfloat level_scale[] = { 2.5, 0.5 };
    draw_image(user_data, TEX_LOC_LEVEL, level_pos, level_scale);

    draw_string(user_data, cleared_lines, 0.835, -0.85);
    GLfloat cleared_lines_pos[]   = { 1.09, -0.73, 0.0 };
    GLfloat cleared_lines_scale[] = { 5.0, 0.5 };
    draw_image(user_data, TEX_LOC_CLEARED_LINES, cleared_lines_pos, cleared_lines_scale);
}

void draw_next_piece(const user_data_t* user_data)
{
    // get position of the next piece according to its type
    GLfloat pos[] = { 1.2, 0.75 };

    glUseProgram(user_data->shader_program_single_block);
    glUniform1f(user_data->block_scale_uniform, 1.0f);

    int block_id = user_data->gameData.next_piece[0] + 1;
    int model_index = block_id + 2;

    glUniform1i(user_data->block_id_uniform, block_id);
    gl_check_error("glUniform1i next_piece");
    glUniform2fv(user_data->block_pos_uniform, 1, pos);
    gl_check_error("glUniform2fv next_piece");

    // draw piece model
    glBindVertexArray(user_data->vao[model_index]);
    glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[model_index]);
    glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count[model_index]);
    gl_check_error("glDrawArrays next_piece");

}

void draw_piece_count(user_data_t* user_data) {
    // position of the piece according to its type
    GLfloat pos[7][2]   = { { -1.5, 0.75 },
                          { -1.5, 0.50 },
                          { -1.5, 0.20 },
                          { -1.5, -0.05 },
                          { -1.5, -0.33 },
                          { -1.5, -0.60 },
                          { -1.5, -0.80 },
                          };

    for (size_t i = 0; i < 7; i++) {
        glUseProgram(user_data->shader_program_single_block);
        glUniform1f(user_data->block_scale_uniform, 0.65f);

        int block_id = i + 1;
        int model_index = block_id + 2;

        glUniform1i(user_data->block_id_uniform, block_id);
        gl_check_error("glUniform1i next_piece");
        glUniform2fv(user_data->block_pos_uniform, 1, pos[i]);
        gl_check_error("glUniform2fv next_piece");

        glBindVertexArray(user_data->vao[model_index]);
        glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[model_index]);
        glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count[model_index]);
        gl_check_error("glDrawArrays next_piece");

        char piece_count[4];
        memset(piece_count, 0, 4);
        sprintf(piece_count, "%03d", user_data->gameData.piece_count[i]);

        draw_string(user_data, piece_count, pos[i][0] + 0.3, pos[i][1]);
    }
}

void draw_gl(GLFWwindow* window)
{
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // Clear the color buffer -> background color:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_check_error("glClear");

    if (user_data->gameData.gameState != GAME_OVER) {
        // draw the arena
        glUseProgram(user_data->shader_program_arena);

        glBindVertexArray(user_data->vao[1]);
        glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[1]);
        glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count[1]);
        gl_check_error("glDrawArrays1");

        // draw the background
        glUseProgram(user_data->shader_program_back);

        glBindVertexArray(user_data->vao[2]);
        glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[2]);
        glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count[2]);
        gl_check_error("glDrawArrays2");

        // draw the pieces
        glBindVertexArray(user_data->vao[0]);
        glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[0]);

        int block_positions[200] = { 0 };
        generate_block_positions(&user_data->gameData, block_positions);

        glUseProgram(user_data->shader_program_blocks);
        glUniform1iv(user_data->block_positions, 200, block_positions);

        // Parameters: primitive type, start index, count
        glDrawArraysInstanced(GL_TRIANGLES, 0, user_data->vertex_data_count[0], 200);
        gl_check_error("glDrawArraysInstanced");

        draw_text(user_data);
        draw_next_piece(user_data);
        draw_piece_count(user_data);

        GLfloat key_map_pos[] = { 0.0, -1.037, 0.0 };
        GLfloat key_map_scale[] = { 20.0, .6 };

        draw_image(user_data, TEX_LOC_KEYMAP, key_map_pos, key_map_scale);
    }

    if (user_data->gameData.gameState == GAME_OVER) {
        GLfloat game_over_pos[] = { 0.0, 0.0, -0.01 };
        GLfloat game_over_scale[] = { 16.0, 9.0 };

        draw_image(user_data, TEX_LOC_GAMEOVER, game_over_pos, game_over_scale);

        char score[7];
        memset(score, 0, 7);
        sprintf(score, "%06d", user_data->gameData.score);
        draw_string(user_data, score, 0.1, 0.0);
    }
}
