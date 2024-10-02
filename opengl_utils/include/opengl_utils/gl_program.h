#ifndef OPENGL_UTILS_GL_PROGRAM_H_
#define OPENGL_UTILS_GL_PROGRAM_H_

#include <epoxy/gl.h>
#include "shader.h"

typedef struct GlProgram {
    GLuint program;
} GlProgram;

GlProgram gl_program_from_2_shaders(const Shader* a, const Shader* b);
GlProgram gl_program_from_sh_and_f(const Shader* a, GLenum b_type,
                                   const char* b_path);
void gl_program_free(GlProgram);

#endif // OPENGL_UTILS_GL_PROGRAM_H_