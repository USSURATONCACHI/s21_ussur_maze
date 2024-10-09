#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_RENDER_SETTINGS_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_RENDER_SETTINGS_VIEW_H_

#include <mazegtk/view_gtk/render_settings_store.h>
#include <gtk/gtk.h>

typedef struct {
    MgRenderSettingsStore* store;

    GtkSpinButton* msaa_coef_btn;

    // GTK does not differ user input and programmatic `_set_value`. 
    // So, we compare input to last set value to check if it is set by the program or by the user.
    struct {
        gdouble msaa_coef;
    } last_shown;
} MgRenderSettingsView;

MgRenderSettingsView* MgRenderSettingsView_create(GtkBuilder* ui, MgRenderSettingsStore* store);
void MgRenderSettingsView_free(MgRenderSettingsView* view);

void MgRenderSettingsView_update_ui(MgRenderSettingsView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_RENDER_SETTINGS_VIEW_H_