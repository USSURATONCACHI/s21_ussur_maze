#ifndef MAZEGTK_VIEWMODEL_H_
#define MAZEGTK_VIEWMODEL_H_

#include <glib-object.h>

#define METHOD(ret, name, ...) ret (*name)(void* this, __VA_ARGS__);
#define PUBLIC(x) x
#define METHODS(x) x
#define PRIVATE(x) struct { x } private;

typedef struct {
    PUBLIC(
        const char* name;
    )

    PRIVATE(
        int state;
    )

    METHODS(
        METHOD(int, add, int a);
    )
} MyClass;

int my_class_add(MyClass* this, int a) {
    this->private.state += a;
    return this->private.state;
}

static void a() {
    MyClass vt = {
        .private = {
            .state = 42,
        },

        .add = (void*) my_class_add,
    };

    vt.add(&vt, 5);
}


typedef enum {
    MAZEGTK_VIEWMODEL_STATE_LOADING,
    MAZEGTK_VIEWMODEL_STATE_DRAG,
    MAZEGTK_VIEWMODEL_STATE_SHOW_MAZE,
} MazegtkViewodelState;

typedef struct {
    char* text_shown;
} MazegtkViewodelStateLoading;
void mg_vm_state_loading_free(MazegtkViewodelStateLoading state);

typedef struct {
} MazegtkViewodelStateDrag;
void mazegtk_viewmodel_state_drag_free(MazegtkViewodelStateDrag state);

typedef struct {
} MazegtkViewodelStateShowMaze;
void mazegtk_viewmodel_state_show_maze_free(MazegtkViewodelStateShowMaze state);

typedef struct {
    MazegtkViewodelState state;
    union {
        MazegtkViewodelStateLoading  loading;
        MazegtkViewodelStateDrag     drag;
        MazegtkViewodelStateShowMaze show_maze;
    };
    struct {
        GCallback on_state_changed;
        void* callback_user_data;
    } private;
} MazegtkViewmodel;

MazegtkViewmodel mazegtk_viewmodel_new();
void mazegtk_viewmodel_free(MazegtkViewmodel model);

void mazegtk_viewmodel_set_user_data(void* callback_user_data);

void mazegtk_viewmodel_on_state_changed(GCallback callback);
void mazegtk_viewmodel_set_state_loading(MazegtkViewodelStateLoading new_state);
void mazegtk_viewmodel_set_state_drag(MazegtkViewodelStateDrag new_state);
void mazegtk_viewmodel_set_state_show_maze(MazegtkViewodelStateShowMaze new_state);

#endif // MAZEGTK_VIEWMODEL_H_