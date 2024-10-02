#ifndef MAZEGTK_VIEWMODEL_STATE_TYPE_H_
#define MAZEGTK_VIEWMODEL_STATE_TYPE_H_

#include <libmaze/maze_struct.h>

typedef enum {
    MAZEGTK_VIEWMODEL_STATE_LOADING,
    MAZEGTK_VIEWMODEL_STATE_DRAG,
    MAZEGTK_VIEWMODEL_STATE_SHOW_MAZE,
} MgViewmodelStateType;

typedef struct {
    char* text_shown;
} MgViewmodelStateLoading;

typedef struct {
} MgViewmodelStateDrag;

typedef struct {
    Maze shown_maze;
} MgViewmodelStateShowMaze;

void mg_viewmodel_state_loading_free(MgViewmodelStateLoading state);
void mg_viewmodel_state_drag_free(MgViewmodelStateDrag state);
void mg_viewmodel_state_show_maze_free(MgViewmodelStateShowMaze state);

#define mg_viewmodel_state_X_free(X) _Generic((X), \
                MgViewmodelStateLoading:  mg_viewmodel_state_loading_free   \
                MgViewmodelStateDrag:     mg_viewmodel_state_drag_free      \
                MgViewmodelStateShowMaze: mg_viewmodel_state_show_maze_free \
            )(X)

#endif // MAZEGTK_VIEWMODEL_STATE_TYPE_H_