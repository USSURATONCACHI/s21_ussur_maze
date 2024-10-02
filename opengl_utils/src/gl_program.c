#include <opengl_utils/gl_program.h>

#include <stdbool.h>

#define LOG_BUF_SIZE (1024 * 512)

GlProgram gl_program_from_2_shaders(const Shader* a, const Shader* b) {
  GLuint program = glCreateProgram();
  glAttachShader(program, a->shader);
  glAttachShader(program, b->shader);
  glLinkProgram(program);

  int success = false;
  char info_log[LOG_BUF_SIZE] = "";
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, LOG_BUF_SIZE, null, info_log);
    panic("Shader linking error: %s", info_log);
  }

  return (GlProgram){
      .program = program,
  };
}
void gl_program_free(GlProgram this) { glDeleteProgram(this.program); }

GlProgram gl_program_from_sh_and_f(const Shader* a, GLenum b_type,
                                   const char* b_path) {
  Shader b = shader_from_file(b_type, b_path);
  GlProgram result = gl_program_from_2_shaders(a, &b);
  shader_free(b);
  return result;
}