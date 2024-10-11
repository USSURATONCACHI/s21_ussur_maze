#include <libmaze/maze_struct.h>
#include <libmaze/vec_size_t.h>
#include <libmaze/ellers_resources.h>
#include <pthread.h>
#include <threads.h>

// Provides non-blocking synchronous API of a thread that runs Eller's algorithm in parallel
typedef struct {
    MzMaze* maze;
    MzEllersResources res;

    pthread_t thread;
    mtx_t mutex;
    bool is_thread_running;

    bool is_allocated;
    size_t rows_done;
} MzEllersOffthreadGenerator;

MzEllersOffthreadGenerator* MzEllersOffthreadGenerator_create();
void MzEllersOffthreadGenerator_free(MzEllersOffthreadGenerator* gen);

bool MzEllersOffthreadGenerator_is_finished(MzEllersOffthreadGenerator* gen);
void MzEllersOffthreadGenerator_start(MzEllersOffthreadGenerator* gen, MzMaze* maze_to_fill);
void MzEllersOffthreadGenerator_terminate(MzEllersOffthreadGenerator* gen);
MzVoidResult MzEllersOffthreadGenerator_await(MzEllersOffthreadGenerator* gen);