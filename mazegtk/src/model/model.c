#include <mazegtk/model/model.h>
#include <time.h>
#include <stdlib.h>

MgModel MgModel_new(size_t maze_width, size_t maze_height) {
    MgModel model = {
        .maze = mz_maze_create(maze_width, maze_height),
        .camera = MgCameraModel_new(maze_width / 2.0f, maze_height / 2.0f),
    };
    srand(time(NULL));
    // mz_maze_fill_random(&model.maze);
    mz_maze_generate_perfect_eller(&model.maze);

    return model;
}
void MgModel_free(MgModel model) {
    mz_maze_free(model.maze);
}