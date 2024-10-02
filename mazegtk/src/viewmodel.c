#include <mazegtk/viewmodel.h>
#include <better_c_std/prettify.h>
#include <libmaze/maze.h>
#include <string.h>
#include <stdlib.h>

MgViewmodelState mg_viewmodel_state_new() {
    return (MgViewmodelState) {
        .type = MAZEGTK_VIEWMODEL_STATE_LOADING,
        .loading = {
            .text_shown = strdup("Starting the application"),
        },
    };
}
void mg_viewmodel_state_free(MgViewmodelState* model) {
    if (model == NULL) return;

    switch (model->type) {
        case MAZEGTK_VIEWMODEL_STATE_LOADING:   mg_viewmodel_state_loading_free  (model->loading);   break;
        case MAZEGTK_VIEWMODEL_STATE_DRAG:      mg_viewmodel_state_drag_free     (model->drag);      break;
        case MAZEGTK_VIEWMODEL_STATE_SHOW_MAZE: mg_viewmodel_state_show_maze_free(model->show_maze); break;
        default:
    }
}

void mg_viewmodel_state_set_loading(MgViewmodelState* self, MgViewmodelStateLoading new_state) {
    if (self == NULL) return;

    mg_viewmodel_state_free(self);
    self->type = MAZEGTK_VIEWMODEL_STATE_LOADING;
    self->loading = new_state;
}
void mg_viewmodel_state_set_drag(MgViewmodelState* self, MgViewmodelStateDrag new_state) {
    if (self == NULL) return;

    mg_viewmodel_state_free(self);
    self->type = MAZEGTK_VIEWMODEL_STATE_DRAG;
    self->drag = new_state;
}
void mg_viewmodel_state_set_show_maze(MgViewmodelState* self, MgViewmodelStateShowMaze new_state) {
    if (self == NULL) return;

    mg_viewmodel_state_free(self);
    self->type = MAZEGTK_VIEWMODEL_STATE_SHOW_MAZE;
    self->show_maze = new_state;
}