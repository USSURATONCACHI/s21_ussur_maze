#include <mazegtk/viewmodel.h>
#include <string.h>

// typedef enum {
//     MAZEGTK_VIEWMODEL_STATE_LOADING,
//     MAZEGTK_VIEWMODEL_STATE_DRAG,
//     MAZEGTK_VIEWMODEL_STATE_SHOW_MAZE,
// } MazegtkViewodelState;

// typedef struct {
//     char* text_shown;
// } MazegtkViewodelStateLoading;

// typedef struct {
// } MazegtkViewodelStateDrag;

// typedef struct {
// } MazegtkViewodelStateShowMaze;

// typedef struct {
//     MazegtkViewodelState state;
//     union {
//         MazegtkViewodelStateLoading  loading;
//         MazegtkViewodelStateDrag     drag;
//         MazegtkViewodelStateShowMaze show_maze;
//     };
//     struct {
//         GCallback on_state_changed;
//         void* callback_user_data;
//     } private;
// } MazegtkViewmodel;

MazegtkViewmodel mazegtk_viewmodel_new() {
    return (MazegtkViewmodel) {
        .state = MAZEGTK_VIEWMODEL_STATE_LOADING,
        .loading = {
            .text_shown = strdup("Launching application"),
        },
        .private = {
            .on_state_changed = NULL,
            .callback_user_data = NULL,
        }
    };
}

void mazegtk_viewmodel_free(MazegtkViewmodel model) {

}

void mazegtk_viewmodel_state_loading_free(MazegtkViewodelStateLoading state) {

}

void mazegtk_viewmodel_state_drag_free(MazegtkViewodelStateDrag state) {

}

void mazegtk_viewmodel_state_show_maze_free(MazegtkViewodelStateShowMaze state) {

}

void mazegtk_viewmodel_set_user_data(void* callback_user_data) {

}

void mazegtk_viewmodel_on_state_changed(GCallback callback) {
    
}
void mazegtk_viewmodel_set_state_loading(MazegtkViewodelStateLoading new_state);
void mazegtk_viewmodel_set_state_drag(MazegtkViewodelStateDrag new_state);
void mazegtk_viewmodel_set_state_show_maze(MazegtkViewodelStateShowMaze new_state);