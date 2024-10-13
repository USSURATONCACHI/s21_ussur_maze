#include <libmaze/a_star.h>
#include <better_c_std/prettify.h>


#include <libmaze/maze_struct.h>


#define VECTOR_C MzCellPos
#include <better_c_std/vector.h>

#define VECTOR_C MzAStarCellData
#include <better_c_std/vector.h>

#define VECTOR_C MzAStarChunk
#define VECTOR_ITEM_DESTRUCTOR MzAStarChunk_free
#include <better_c_std/vector.h>

MzAStarChunk MzAStarChunk_create(size_t start_x, size_t start_y, size_t width, size_t height) {
    size_t cells_count = height * width;
    vec_MzAStarCellData buffer = vec_MzAStarCellData_with_capacity(cells_count);
    buffer.length = buffer.capacity;
    memset(buffer.data, 0, buffer.length * sizeof(MzAStarCellData));

    return (MzAStarChunk) {
        .cells_buffer = buffer,
        .start_x      = start_x,
        .start_y      = start_y,
        .width        = width,
        .height       = height,
    };
}
void MzAStarChunk_free(MzAStarChunk self) {
    vec_MzAStarCellData_free(self.cells_buffer);
}

void MzAStarChunk_set(MzAStarChunk* self, size_t x, size_t y, MzAStarCellData data) {
    assert_m(x >= self->start_x && x < self->start_x + self->width);
    assert_m(y >= self->start_y && y < self->start_y + self->height);

    size_t gx = x - self->start_x;
    size_t gy = y - self->start_y;

    self->cells_buffer.data[gy * self->width + gx] = data;
}
MzAStarCellData MzAStarChunk_get(MzAStarChunk* self, size_t x, size_t y) {
    assert_m(x >= self->start_x && x < self->start_x + self->width);
    assert_m(y >= self->start_y && y < self->start_y + self->height);

    size_t gx = x - self->start_x;
    size_t gy = y - self->start_y;

    return self->cells_buffer.data[gy * self->width + gx];
}

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

MzAStarChunk MzAStarChunk_merge(MzAStarChunk a, MzAStarChunk b) {
    size_t start_x = MIN(a.start_x, b.start_x);
    size_t start_y = MIN(a.start_y, b.start_y);

    size_t end_x = MAX(a.start_x + a.width, b.start_x + b.width);
    size_t end_y = MAX(a.start_y + a.height, b.start_y + b.height);

    MzAStarChunk chunk = MzAStarChunk_create(start_x, start_y, end_x - start_x, end_y - start_y);

    for (size_t y = a.start_y; y < a.start_y + a.height; y++)
        for (size_t x = a.start_x; x < a.start_x + a.width; x++)
            MzAStarChunk_set(&chunk, x, y, MzAStarChunk_get(&a, x, y));

    for (size_t y = b.start_y; y < b.start_y + b.height; y++)
        for (size_t x = b.start_x; x < b.start_x + b.width; x++)
            MzAStarChunk_set(&chunk, x, y, MzAStarChunk_get(&b, x, y));

    MzAStarChunk_free(a);
    MzAStarChunk_free(b);

    return chunk;
}


MzAStarPathfinder MzAStarPathfinder_create(size_t start_x, size_t start_y, size_t end_x, size_t end_y) {
    return (MzAStarPathfinder) {
        .cells_to_check = vec_MzCellPos_create(),
        .checked_cells = vec_MzAStarChunk_create(),
        .start_x = start_x,
        .start_y = start_y,
        .end_x = end_x,
        .end_y = end_y,
    };
}

void MzAStarPathfinder_free(MzAStarPathfinder self) {
    vec_MzAStarChunk_free(self.checked_cells);
    vec_MzCellPos_free(self.cells_to_check);
}

MzAStarCellData MzAStarPathfinder_get_cell_at(MzAStarPathfinder* self, size_t x, size_t y) {
    for (size_t i = 0; i < self->checked_cells.length; i++) {
        MzAStarChunk* chunk = vec_MzAStarChunk_atref(&self->checked_cells, i);

        bool x_inside = x >= chunk->start_x && x <= (chunk->start_x + chunk->width);
        bool y_inside = y >= chunk->start_y && y <= (chunk->start_y + chunk->height);

        if (x_inside && y_inside)
            return MzAStarChunk_get(chunk, x, y);
    }

    return (MzAStarCellData) {
        .g_value = 0,
        .h_value = 0,
        .parent_pos = SIZE_MAX,
        .is_init = false,
    };
}

#define DEFAULT_CHUNK_SIZE 64

void MzAStarPathfinder_set_cell_at(MzAStarPathfinder* self, size_t x, size_t y, MzAStarCellData cell) {
    for (size_t i = 0; i < self->checked_cells.length; i++) {
        MzAStarChunk* chunk = vec_MzAStarChunk_atref(&self->checked_cells, i);

        bool x_inside = x >= chunk->start_x && x <= (chunk->start_x + chunk->width);
        bool y_inside = y >= chunk->start_y && y <= (chunk->start_y + chunk->height);

        if (x_inside && y_inside) {
            MzAStarChunk_set(chunk, x, y, cell);
            return;
        }
    }

    // Add new chunk
    size_t chunk_x = x / DEFAULT_CHUNK_SIZE;
    size_t chunk_y = y / DEFAULT_CHUNK_SIZE;

    size_t chunk_start_x = chunk_x * DEFAULT_CHUNK_SIZE;
    size_t chunk_start_y = chunk_y * DEFAULT_CHUNK_SIZE;

    MzAStarChunk new_chunk = MzAStarChunk_create(chunk_start_x, chunk_start_y, DEFAULT_CHUNK_SIZE, DEFAULT_CHUNK_SIZE);
    MzAStarChunk_set(&new_chunk, x, y, cell);
    vec_MzAStarChunk_push(&self->checked_cells, new_chunk);
}

void MzAStarPathfinder_update_chunks(MzAStarPathfinder* self) {
    // nothing, currently
    unused(self);
}

void MzAStarPathfinder_pathfind(MzAStarPathfinder* self, const MzMaze* maze) {
    unused(self);
    unused(maze);
    debugln("TODO!");
}