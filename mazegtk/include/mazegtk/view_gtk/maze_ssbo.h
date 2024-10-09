#ifndef MAZEGTK_VIEW_MAZE_SSBO_H_
#define MAZEGTK_VIEW_MAZE_SSBO_H_

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t data_buffer;
    uint32_t size_buffer;
} MazeSsbo;

MazeSsbo MazeSsbo_create();
void MazeSsbo_upload(const MazeSsbo* ssbo, size_t width, size_t height, size_t data_size, void* data);
void MazeSsbo_free(MazeSsbo ssbo);

#define VECTOR_ITEM_TYPE MazeSsbo
#include <better_c_std/vector.h>


#endif // MAZEGTK_VIEW_MAZE_SSBO_H_