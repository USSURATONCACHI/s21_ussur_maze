#include <mazegtk/viewmodel_state.h>
#include <better_c_std/prettify.h>
#include <stdlib.h>
#include <libmaze/maze.h>

void mg_viewmodel_state_loading_free(MgViewmodelStateLoading state) {
    free(state.text_shown);
}
void mg_viewmodel_state_drag_free(MgViewmodelStateDrag state) {
    // nothing
}
void mg_viewmodel_state_show_maze_free(MgViewmodelStateShowMaze state) {
    maze_free(state.shown_maze);
}