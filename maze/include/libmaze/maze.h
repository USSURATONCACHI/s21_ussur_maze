#ifndef LIBMAZE_MAZE_H_
#define LIBMAZE_MAZE_H_

#include <stdbool.h>
#include <stddef.h>

#include <better_c_std/io/out_stream.h>
#include <better_c_std/pipe.h>

#include <libmaze/maze_struct.h>
#include <libmaze/cell_struct.h>
#include <libmaze/directions_struct.h>
#include <libmaze/ellers_resources.h>
#include <libmaze/cell_pos.h>

// Ctor & destructor
MzMazeResult MzMaze_create(size_t width, size_t height);
void MzMaze_free(MzMaze maze);

// General methods
void MzMaze_set_at(MzMaze* maze, size_t x, size_t y, MzCell value);
MzCell MzMaze_at(const MzMaze* maze, size_t x, size_t y);
MzDirections MzMaze_where_can_go(const MzMaze* maze, size_t at_x, size_t at_y);
size_t MzMaze_get_buffer_size(const MzMaze* maze);

// -- Printing
void MzMaze_print(const MzMaze* maze);
void MzMaze_print_with_path(const MzMaze* maze, const vec_MzCellPos* path);

// -- Serialization
void MzMaze_serialize_binary(const MzMaze* maze, OutStream stream);
void MzMaze_serialize_text(const MzMaze* maze, OutStream stream);

typedef STRUCT_RESULT(MzMaze, BcstdStr) MzMazeReadResult;
MzMazeReadResult MzMaze_read_binary(BcstdPipeHandle* in_stream);
MzMazeReadResult MzMaze_read_text(BcstdPipeHandle* in_stream);

// -- Random fill
void MzMaze_fill_random(MzMaze* maze);

// -- Eller's algorithm
MzMazeResult MzMaze_create_perfect_eller(size_t width, size_t height);
MzVoidResult MzMaze_fill_perfect_eller(MzMaze* maze);
void MzMaze_fill_perfect_eller_preallocated(MzMaze* maze, MzEllersResources* res, size_t* current_row_report);

// -- Misc
MzMazeResult MzMaze_crop_to_size(const MzMaze* old_one, size_t new_width, size_t new_height);



#endif // LIBMAZE_MAZE_H_