#ifndef ERROR_H_
#define ERROR_H_

#include <glad/glad.h>
#include <stdio.h>
#include <stdbool.h>

void check_error(bool condition, const char* error_text);
void gl_check_error(const char* error_text);

#endif