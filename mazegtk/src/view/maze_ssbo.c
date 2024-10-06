#include <epoxy/gl.h>
#include <mazegtk/view/maze_ssbo.h>

MazeSsbo MazeSsbo_create() {
    GLuint buffers[2];
    glGenBuffers(2, buffers);
    return (MazeSsbo) {
        .data_buffer = buffers[0],
        .size_buffer = buffers[1],
    };
}

void MazeSsbo_upload(const MazeSsbo* ssbo, size_t width, size_t height, size_t data_size, void* data) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->data_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, data_size, data, GL_STATIC_DRAW);

    GLuint maze_size[2] = { width, height };
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->size_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(maze_size), maze_size, GL_STATIC_DRAW);
 
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void MazeSsbo_free(MazeSsbo ssbo) {
    GLuint buffers[2] = {
        ssbo.data_buffer,
        ssbo.size_buffer
    };
    glDeleteBuffers(2, buffers);
}

#define VECTOR_C MazeSsbo
#define VECTOR_ITEM_DESTRUCTOR MazeSsbo_free 
#include <better_c_std/vector.h>