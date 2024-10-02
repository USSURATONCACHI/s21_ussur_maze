#ifndef MAZEGTK_VIEWMODEL_H_
#define MAZEGTK_VIEWMODEL_H_

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
} MgViewmodelStateShowMaze;

void mg_viewmodel_state_loading_free(MgViewmodelStateLoading state);
void mg_viewmodel_state_drag_free(MgViewmodelStateDrag state);
void mg_viewmodel_state_show_maze_free(MgViewmodelStateShowMaze state);

#define mg_viewmodel_state_X_free(X) _Generic((X), \
                MgViewmodelStateLoading:  mg_viewmodel_state_loading_free   \
                MgViewmodelStateDrag:     mg_viewmodel_state_drag_free      \
                MgViewmodelStateShowMaze: mg_viewmodel_state_show_maze_free \
            )(X)

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

#define mg_viewmodel_state_set(X) _Generic((X), \
                MgViewmodelStateLoading:  mg_viewmodel_state_set_loading   \
                MgViewmodelStateDrag:     mg_viewmodel_state_set_drag      \
                MgViewmodelStateShowMaze: mg_viewmodel_state_set_show_maze \
            )(X)


#endif // MAZEGTK_VIEWMODEL_H_