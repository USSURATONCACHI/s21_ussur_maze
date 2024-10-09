#ifndef MAZEGTK_VIEW_GTK_RENDER_SETTINGS_STORE_H_
#define MAZEGTK_VIEW_GTK_RENDER_SETTINGS_STORE_H_

#include <stdlib.h>

// It only makes sense to store view-specific state in the view itself
typedef struct {
    long double msaa_resolution_coef;
} MgRenderSettingsStore;

// I doubt such a simple thing needs a translation unit. Inline it goes.
static inline MgRenderSettingsStore* MgRenderSettingsStore_new() {
    MgRenderSettingsStore* store = (void*) malloc(sizeof(MgRenderSettingsStore));

    *store = (MgRenderSettingsStore){
        .msaa_resolution_coef = 4.0,
    };

    return store;
}
static inline void MgRenderSettingsStore_free(MgRenderSettingsStore* store) {
    free(store);
}

#endif // MAZEGTK_VIEW_GTK_RENDER_SETTINGS_STORE_H_