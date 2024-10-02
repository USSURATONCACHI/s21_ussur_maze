#ifndef OPENGL_UTILS_SHADER_H_
#define OPENGL_UTILS_SHADER_H_
#include <epoxy/gl.h>

typedef struct Shader {
    GLuint shader;
} Shader;

Shader shader_from_source(GLenum type, const char* source);
Shader shader_from_file(GLenum type, const char* filepath);
void shader_free(Shader);

#endif // OPENGL_UTILS_SHADER_H_