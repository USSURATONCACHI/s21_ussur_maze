#ifndef MAZEGTK_VIEWMODEL_STATE_H_
#define MAZEGTK_VIEWMODEL_STATE_H_

#include <libmaze/maze_struct.h>
#include <mazegtk/viewmodel_state.h>

typedef struct {
    MgViewmodelStateType type;
    union {
        MgViewmodelStateLoading  loading;
        MgViewmodelStateDrag     drag;
        MgViewmodelStateShowMaze show_maze;
    };
} MgViewmodelState;

MgViewmodelState mg_viewmodel_state_new();
void mg_viewmodel_state_free(MgViewmodelState* self);

void mg_viewmodel_state_set_loading  (MgViewmodelState* self, MgViewmodelStateLoading new_state);
void mg_viewmodel_state_set_drag     (MgViewmodelState* self, MgViewmodelStateDrag new_state);
void mg_viewmodel_state_set_show_maze(MgViewmodelState* self, MgViewmodelStateShowMaze new_state);

#define mg_viewmodel_state_set(vm, X) _Generic((X), \
                MgViewmodelStateLoading:  mg_viewmodel_state_set_loading,   \
                MgViewmodelStateDrag:     mg_viewmodel_state_set_drag,      \
                MgViewmodelStateShowMaze: mg_viewmodel_state_set_show_maze \
            )(vm, X)


#endif // MAZEGTK_VIEWMODEL_STATE_H_