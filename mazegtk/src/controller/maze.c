#include <mazegtk/controller/maze.h>
#include <better_c_std/camera/camera.h>
#include <better_c_std/prettify.h>

#include <libmaze/maze.h>

MgMazeController* MgMazeController_new(MzMaze* maze) {
    return (MgMazeController*)maze;
}
void MgMazeController_free(MgMazeController* controller) {
    unused(controller);
}

uint8_t* MgMazeController_data_buffer(MgMazeController* self_in) {
    MzMaze* maze = (void*)self_in;
    return maze->raw_data;
}
size_t MgMazeController_data_size(MgMazeController* self_in) {
    MzMaze* maze = (void*)self_in;
    return mz_maze_get_buffer_size(maze);
}
size_t MgMazeController_width(MgMazeController* self_in) {
    MzMaze* maze = (void*)self_in;
    return maze->width;
}
size_t MgMazeController_height(MgMazeController* self_in) {
    MzMaze* maze = (void*)self_in;
    return maze->height;
}