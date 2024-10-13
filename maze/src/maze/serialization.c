#include <libmaze/maze.h>
#include <better_c_std/prettify.h>

void MzMaze_serialize_binary(const MzMaze* maze, OutStream stream) {
    uint64_t width = maze->width;
    uint64_t height = maze->height;
    OutStream_write(stream, &width, sizeof(uint64_t));
    OutStream_write(stream, &height, sizeof(uint64_t));

    size_t buf_size = MzMaze_get_buffer_size(maze);
    OutStream_write(stream, maze->raw_data, buf_size);
}
void MzMaze_serialize_text(const MzMaze* maze, OutStream stream) {
    unused(maze);
    unused(stream);
    debugln("TODO!");
}
MzMazeReadResult MzMaze_read_binary(BcstdPipeHandle* in_stream) {
    uint64_t width, height;
    
    BcstdReadResult res = BcstdPipeHandle_read(in_stream, &width, sizeof(uint64_t));
    if (!res.is_ok)
        return (MzMazeReadResult) ERR(res.error);
    if (res.ok != sizeof(uint64_t))
        return (MzMazeReadResult) ERR(BcstdStr_literal("Not enough data to read"));

    res = BcstdPipeHandle_read(in_stream, &height, sizeof(uint64_t));
    if (!res.is_ok)
        return (MzMazeReadResult) ERR(res.error);
    if (res.ok != sizeof(uint64_t))
        return (MzMazeReadResult) ERR(BcstdStr_literal("Not enough data to read"));

    MzMazeResult maze_res = MzMaze_create(width, height);
    if (!maze_res.is_ok) {
        BcstdStr error;
        switch (maze_res.error) {
            case MZ_ERROR_NOT_ENOUGH_MEMORY:
                error = BcstdStr_literal("Not enough memory");
                break;
            default: 
                error = BcstdStr_literal("Unknown MzError");
        }
        return (MzMazeReadResult) ERR(error);
    }
    MzMaze maze = maze_res.ok;
    size_t buffer_size = MzMaze_get_buffer_size(&maze);
    res = BcstdPipeHandle_read(in_stream, maze.raw_data, buffer_size);
    if (!res.is_ok) {
        MzMaze_free(maze);
        return (MzMazeReadResult) ERR(res.error);
    }
    if (res.ok != buffer_size) {
        MzMaze_free(maze);
        return (MzMazeReadResult) ERR(BcstdStr_literal("Not enough data to read"));
    }

    return (MzMazeReadResult) OK(maze);
}
MzMazeReadResult MzMaze_read_text(BcstdPipeHandle* in_stream) {
    unused(in_stream);
    panic("TODO");
}
