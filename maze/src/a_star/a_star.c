#include <libmaze/a_star.h>
#include <libmaze/maze.h>
#include <better_c_std/prettify.h>


#include <libmaze/maze_struct.h>

#define VECTOR_C MzAStarCellToCheck
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
        .cells_to_check = vec_MzAStarCellToCheck_create(),
        .checked_cells = vec_MzAStarChunk_create(),
        .start_x = start_x,
        .start_y = start_y,
        .end_x = end_x,
        .end_y = end_y,
    };
}

void MzAStarPathfinder_free(MzAStarPathfinder self) {
    vec_MzAStarChunk_free(self.checked_cells);
    vec_MzAStarCellToCheck_free(self.cells_to_check);
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
        .parent_x = SIZE_MAX,
        .parent_y = SIZE_MAX,
        .g_value = 0,
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

static size_t heuristics_manhattan(size_t ax, size_t ay, size_t bx, size_t by) {
    return (MAX(bx, ax) - MIN(bx, ax)) + (MAX(by, ay) - MIN(by, ay));
}
static size_t f_value(MzAStarCellToCheck pos) {
    return pos.g_value + pos.h_value;
}

static void MzAStarPathfinder_add_successors(MzAStarPathfinder* self, const MzMaze* maze, size_t x, size_t y);

vec_MzCellPos MzAStarPathfinder_pathfind(MzAStarPathfinder* self, const MzMaze* maze) {
    unused(self);
    unused(maze);

    // Add first cell
    MzAStarCellData init_cell = {
        .g_value = 0,
        .is_init = true,
        .parent_x = SIZE_MAX,
        .parent_y = SIZE_MAX,
    };

    MzAStarPathfinder_set_cell_at(self, self->start_x, self->start_y, init_cell);

    // Add all possible successors of fist cell
    MzAStarPathfinder_add_successors(self, maze, self->start_x, self->start_y);

    debugln("Chunks count: %zu, cells to check: %zu", self->checked_cells.length, self->cells_to_check.length);

    size_t check_num = 0;
    bool end_found = false;
    while (!end_found && self->cells_to_check.length > 0) {
        size_t min_f_value = f_value(self->cells_to_check.data[0]);
        size_t min_index = 0;

        for (size_t i = 1; i < self->cells_to_check.length; i++) {
            size_t f = f_value(self->cells_to_check.data[i]);
            if (f < min_f_value) {
                min_f_value = f;
                min_index = i;
            }
        }
        
        MzAStarCellToCheck cell_to_check = vec_MzAStarCellToCheck_extract_fast(&self->cells_to_check, min_index);
        check_num++;
        debugln("Checking (%zu) XY: %zu %zu (<- %zu %zu)", check_num, cell_to_check.x, cell_to_check.y, cell_to_check.parent_x, cell_to_check.parent_y);

        MzAStarCellData its_data = MzAStarPathfinder_get_cell_at(self, cell_to_check.x, cell_to_check.y);
        if (its_data.is_init)
            continue;

        if (cell_to_check.x == self->end_x && cell_to_check.y == self->end_y) {
            debugln("End found");
            end_found = true;
        }

        MzAStarCellData parent_data = MzAStarPathfinder_get_cell_at(self, cell_to_check.parent_x, cell_to_check.parent_y);
        MzAStarCellData this_cell = {
            .parent_x = cell_to_check.parent_x,
            .parent_y = cell_to_check.parent_y,
            .g_value = parent_data.g_value + 1,
            .is_init = true,
        };
        MzAStarPathfinder_set_cell_at(self, cell_to_check.x, cell_to_check.y, this_cell);
        MzAStarPathfinder_add_successors(self, maze, cell_to_check.x, cell_to_check.y);
    }

    assert_m(end_found);    
    vec_MzCellPos path = vec_MzCellPos_with_capacity(maze->width + maze->height);
    
    size_t x = self->end_x;
    size_t y = self->end_y;
    do {
        vec_MzCellPos_push(&path, (MzCellPos) { x, y });
        MzAStarCellData head = MzAStarPathfinder_get_cell_at(self, x, y);
        assert_m(head.is_init);

        if (x == self->start_x && y == self->start_y)
            break;

        debugln("X %zu Y %zu <- X %zu Y %zu", x, y, head.parent_x, head.parent_y);
        x = head.parent_x;
        y = head.parent_y;
    } while (x != SIZE_MAX && y != SIZE_MAX);

    for (size_t i = 0; i < path.length / 2; i++)
        SWAP(MzCellPos, path.data[i], path.data[path.length - 1 - i]);

    return path;
}


static void MzAStarPathfinder_add_successors(MzAStarPathfinder* self, const MzMaze* maze, size_t x, size_t y) {
    MzAStarCellData cell = MzAStarPathfinder_get_cell_at(self, x, y);
    assert_m(cell.is_init);

    MzCell maze_cell = MzMaze_at(maze, x, y);

    if (x > 0 && !maze_cell.left_wall) {
        // add left successor
        MzAStarCellToCheck pos = {
            .parent_x = x,
            .parent_y = y,
            .x = x - 1,
            .y = y,
            .h_value = heuristics_manhattan(x - 1, y, self->end_x, self->end_y),
            .g_value = cell.g_value + 1,
        };
        if (!MzAStarPathfinder_get_cell_at(self, x - 1, y).is_init)
            vec_MzAStarCellToCheck_push(&self->cells_to_check, pos);
    }
    if (y > 0 && !maze_cell.top_wall) {
        // add top successor
        MzAStarCellToCheck pos = {
            .parent_x = x,
            .parent_y = y,
            .x = x,
            .y = y - 1,
            .h_value = heuristics_manhattan(x, y - 1, self->end_x, self->end_y),
            .g_value = cell.g_value + 1,
        };
        if (!MzAStarPathfinder_get_cell_at(self, x, y - 1).is_init)
            vec_MzAStarCellToCheck_push(&self->cells_to_check, pos);
    }

    if ((x + 1) < maze->width) {
        MzCell right_cell = MzMaze_at(maze, x + 1, y);
        if (!right_cell.left_wall) {
            // add left successor
            MzAStarCellToCheck pos = {
                .parent_x = x,
                .parent_y = y,
                .x = x + 1,
                .y = y,
                .h_value = heuristics_manhattan(x + 1, y, self->end_x, self->end_y),
                .g_value = cell.g_value + 1,
            };
            if (!MzAStarPathfinder_get_cell_at(self, x + 1, y).is_init)
                vec_MzAStarCellToCheck_push(&self->cells_to_check, pos);
        }
    }
    if ((y + 1) < maze->height) {
        MzCell bottom_cell = MzMaze_at(maze, x, y + 1);
        if (!bottom_cell.top_wall) {
            // add bottom successor
            MzAStarCellToCheck pos = {
                .parent_x = x,
                .parent_y = y,
                .x = x,
                .y = y + 1,
                .h_value = heuristics_manhattan(x, y + 1, self->end_x, self->end_y),
                .g_value = cell.g_value + 1,
            };
            if (!MzAStarPathfinder_get_cell_at(self, x, y + 1).is_init)
                vec_MzAStarCellToCheck_push(&self->cells_to_check, pos);
        }
    }

}