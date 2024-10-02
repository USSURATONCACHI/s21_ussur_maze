#include <opengl_utils/shader.h>

#include <stdbool.h>

#define LOG_BUF_SIZE (1024 * 512)

Shader shader_from_source(GLenum type, const char* source);
void shader_free(Shader this) { glDeleteShader(this.shader); }

Shader shader_from_source(GLuint type, const char* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int success = false;
  char info_log[LOG_BUF_SIZE] = "";
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, LOG_BUF_SIZE, NULL, info_log);
    panic("Shader compilation error: %s", info_log);
  }

  return (Shader){.shader = shader};
}