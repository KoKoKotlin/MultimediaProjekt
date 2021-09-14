#include "error.h"

#include "glad/glad.h"

void check_error(bool condition, const char* error_text)
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
