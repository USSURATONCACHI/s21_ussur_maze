#ifndef MAZEGTK_CONTROLLER_CONTROLLER_H_
#define MAZEGTK_CONTROLLER_CONTROLLER_H_

#include <stdint.h>
#include <mazegtk/controller/camera.h>
#include <mazegtk/controller/maze.h>
#include <mazegtk/controller/maze_gen.h>
#include <mazegtk/model/model.h>

// Opaque pointers are used instead of full defined structs to
// not encourage coders (me) to use parts of inner implementation in view layer.
// This way, internals are simply inaccessible.
typedef struct MgController MgController;

MgController* MgController_new(MgModel* model_ref);
void MgController_free(MgController* controller);

MgMazeController*    MgController_get_maze(MgController* self);
MgMazeGenController* MgController_get_maze_gen(MgController* self);
MgCameraController*  MgController_get_camera(MgController* self);

// typedef enum {
//     MG_SUCCESS,
//     MG_NOT_ENOUGH_MEMORY,
// } MgCreateError; 

// MgCreateError MgController_create_maze_empty(MgController* self, size_t maze_width, size_t maze_height);
// MgCreateError MgController_create_maze_eller(MgController* self, size_t maze_width, size_t maze_height);
// MgCreateError MgController_create_maze_random(MgController* self, size_t maze_width, size_t maze_height);


#endif // MAZEGTK_CONTROLLER_CONTROLLER_H_