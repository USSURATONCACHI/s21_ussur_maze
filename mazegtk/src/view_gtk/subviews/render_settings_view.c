#include <mazegtk/view_gtk/subviews/render_settings_view.h>
#include <mazegtk/view_gtk/subviews/ui_update_macros.h>
#include <better_c_std/prettify.h>

static void h_msaa_coef_changed(GtkSpinButton* widget, MgRenderSettingsView* view);

MgRenderSettingsView* MgRenderSettingsView_create(GtkBuilder* ui, MgRenderSettingsStore* store) {
    if (ui == NULL || store == NULL)
        return NULL;

    MgRenderSettingsView* view = (void*) malloc(sizeof(MgRenderSettingsView));
    assert_alloc(view);

    *view = (MgRenderSettingsView) {
        .store = store,
        .msaa_coef_btn = GTK_SPIN_BUTTON(gtk_builder_get_object(ui, "msaa_coef")),
        .last_shown = {}, // can be uninitialized, does not matter 
    };

    if (view->msaa_coef_btn) g_signal_connect(view->msaa_coef_btn, "value-changed", G_CALLBACK(h_msaa_coef_changed), view);

    return view;
}

void MgRenderSettingsView_free(MgRenderSettingsView* view) {
    if (view == NULL)
        return;

    if (view->msaa_coef_btn) g_signal_handlers_disconnect_by_data(view->msaa_coef_btn, view);
    free(view);
}

void MgRenderSettingsView_update_ui(MgRenderSettingsView* view) {
    UPDATE_SPINBTN_GDOUBLE(view->msaa_coef_btn, view->last_shown.msaa_coef, view->store->msaa_resolution_coef);
}

static void h_msaa_coef_changed(GtkSpinButton* widget, MgRenderSettingsView* view) {
    CHECK_GDOUBLE_CHANGED(val, widget, view->store->msaa_resolution_coef);
    view->store->msaa_resolution_coef = val;    
}