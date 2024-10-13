#ifndef LIBMAZE_A_STAR_H_
#define LIBMAZE_A_STAR_H_

#include <libmaze/maze_struct.h>
#include <libmaze/cell_pos.h>

typedef struct {
    size_t parent_x, parent_y;
    size_t x, y;
    size_t h_value, g_value;
} MzAStarCellToCheck;

#define VECTOR_ITEM_TYPE MzAStarCellToCheck
#include <better_c_std/vector.h>

typedef struct {
    size_t parent_x, parent_y;
    uint32_t g_value; // same as in dijkstra's algorithm
    bool is_init;
} MzAStarCellData;

#define VECTOR_ITEM_TYPE MzAStarCellData
#include <better_c_std/vector.h>

typedef struct {
    vec_MzAStarCellData cells_buffer;
    size_t start_x;
    size_t start_y;
    size_t width;
    size_t height;
} MzAStarChunk;

MzAStarChunk MzAStarChunk_create(size_t start_x, size_t start_y, size_t width, size_t height);
void MzAStarChunk_free(MzAStarChunk self);

MzAStarChunk MzAStarChunk_merge(MzAStarChunk a, MzAStarChunk b);
void MzAStarChunk_set(MzAStarChunk* self, size_t x, size_t y, MzAStarCellData data);
MzAStarCellData MzAStarChunk_get(MzAStarChunk* self, size_t x, size_t y);

#define VECTOR_ITEM_TYPE MzAStarChunk
#include <better_c_std/vector.h>

// If we do not do chunking system, solving mere 10k by 10k maze could easily require 1.4 GiB or RAM at least.
typedef struct {
    vec_MzAStarChunk checked_cells;
    vec_MzAStarCellToCheck cells_to_check;

    size_t start_x;
    size_t start_y;

    size_t end_x;
    size_t end_y;
} MzAStarPathfinder;

MzAStarPathfinder MzAStarPathfinder_create(size_t start_x, size_t start_y, size_t end_x, size_t end_y);
void MzAStarPathfinder_free(MzAStarPathfinder self);

MzAStarCellData MzAStarPathfinder_get_cell_at(MzAStarPathfinder* self, size_t x, size_t y);
void MzAStarPathfinder_set_cell_at(MzAStarPathfinder* self, size_t x, size_t y, MzAStarCellData cell);
void MzAStarPathfinder_update_chunks(MzAStarPathfinder* self);

vec_MzCellPos MzAStarPathfinder_pathfind(MzAStarPathfinder* self, const MzMaze* maze);

#endif // LIBMAZE_A_STAR_H_