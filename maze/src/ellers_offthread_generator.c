#include <libmaze/ellers_offthread_generator.h>
#include <libmaze/maze.h>
#include <better_c_std/prettify.h>
#include <signal.h>

static MzVoidResult* thread_fn(MzEllersOffthreadGenerator* self);

MzEllersOffthreadGenerator* MzEllersOffthreadGenerator_create(MzMaze* maze) {
    MzEllersResourcesResult res = MzEllersResources_allocate(0); // thread will later reinit that
    if (!res.is_ok)
        return NULL;

    MzEllersOffthreadGenerator* gen = (void*) malloc(sizeof(MzEllersOffthreadGenerator));
    assert_alloc(gen);
    *gen = (MzEllersOffthreadGenerator) {
        .maze = maze,
        .res = res.ok,
        // thread - uninit
        // mutex - init later
        .is_thread_running = false,
        .is_allocated = false,
        .rows_done = 0,
    };
    assert_m(mtx_init(&gen->mutex, mtx_plain) == thrd_success);

    return gen;
}

void MzEllersOffthreadGenerator_free(MzEllersOffthreadGenerator* gen) {
    MzEllersOffthreadGenerator_terminate(gen);

    mtx_destroy(&gen->mutex);
    MzEllersResources_free(gen->res);
}

bool MzEllersOffthreadGenerator_is_finished(MzEllersOffthreadGenerator* gen) {
    mtx_lock(&gen->mutex);
    bool result = !gen->is_thread_running;
    mtx_unlock(&gen->mutex);
    return result;
}

void MzEllersOffthreadGenerator_start(MzEllersOffthreadGenerator* gen) {
    mtx_lock(&gen->mutex);
    if (!gen->is_thread_running) {
        gen->is_allocated = false;
        gen->rows_done = 0;

        pthread_create(&gen->thread, NULL, (void*)thread_fn, gen);
        gen->is_thread_running = true;
    }
    mtx_unlock(&gen->mutex);
}

void MzEllersOffthreadGenerator_terminate(MzEllersOffthreadGenerator* gen) {
    mtx_lock(&gen->mutex);
    if (gen->is_thread_running) {
        pthread_kill(gen->thread, SIGKILL);
        gen->is_thread_running = false;
    }
    mtx_unlock(&gen->mutex);
}

MzVoidResult MzEllersOffthreadGenerator_await(MzEllersOffthreadGenerator* gen) {
    MzVoidResult* heap_result;
    
    mtx_lock(&gen->mutex);
    pthread_join(gen->thread, (void**) &heap_result);
    mtx_unlock(&gen->mutex);

    assert_m(heap_result != NULL);
    MzVoidResult result = *heap_result;
    free(heap_result);
    return result;
}

#define RETURN_HEAP(type, result) \
    {                                                  \
        type* ret_value = (type*)malloc(sizeof(type)); \
        assert_alloc(ret_value);                       \
        *ret_value = (type) result;                    \
        return ret_value;                              \
    }                                                  \
    //;

static MzVoidResult* thread_fn(MzEllersOffthreadGenerator* self) {
    // Allocate resources with correct width
    mtx_lock(&self->mutex);
    MzEllersResourcesResult new_res = MzEllersResources_allocate(self->maze->width);
    if (!new_res.is_ok) {
        mtx_unlock(&self->mutex);
        RETURN_HEAP(  MzVoidResult, ERR(new_res.error)  );
    } else {
        MzEllersResources_free(self->res);
        self->res = new_res.ok;
        self->is_allocated = true;
        mtx_unlock(&self->mutex);
    }

    // Start the algorithm
    MzMaze_fill_perfect_eller_preallocated(self->maze, &self->res, &self->rows_done);

    // Everything finished
    mtx_lock(&self->mutex);
    self->is_thread_running = false;
    mtx_unlock(&self->mutex);
    RETURN_HEAP(  MzVoidResult, OK({})  );
}