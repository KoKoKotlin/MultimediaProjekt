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

void draw_text(const user_data_t* user_data)
{
    char score[7];
    memset(score, 0, 7);
    char level[3];
    memset(level, 0, 3);
    char cleared_lines[7];
    memset(cleared_lines, 0, 7);

    sprintf(score, "%06d", user_data->gameData.score);
    sprintf(level, "%02d", user_data->gameData.level);
    sprintf(cleared_lines, "%06d", user_data->gameData.cleared_lines);

    draw_string(user_data, score, 0.835, 0.1);
    draw_string(user_data, level, 1.1, -0.38);
    draw_string(user_data, cleared_lines, 0.835, -0.85);
}

void draw_next_piece(const user_data_t* user_data, double pos_x, double pos_y)
{
    // set position and texture for next char
    GLfloat pos[] = { pos_x, pos_y };
    glUseProgram(user_data->shader_program_single_block);

    int block_id = user_data->gameData.next_piece[0] + 1;
    int model_index = block_id + 2;

    glUniform1i(user_data->block_id_uniform, block_id);
    gl_check_error("glUniform1i next_piece");
    glUniform2fv(user_data->block_pos_uniform, 1, pos);
    gl_check_error("glUniform2fv next_piece");

    // draw single char
    glBindVertexArray(user_data->vao[model_index]);
    glBindBuffer(GL_ARRAY_BUFFER, user_data->vbo[model_index]);
    glDrawArrays(GL_TRIANGLES, 0, user_data->vertex_data_count[model_index]);
    gl_check_error("glDrawArrays next_piece");

}

void draw_gl(GLFWwindow* window)
{
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // Clear the color buffer -> background color:
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gl_check_error("glClear");

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
    draw_next_piece(user_data, 1.2, 0.75);
}
