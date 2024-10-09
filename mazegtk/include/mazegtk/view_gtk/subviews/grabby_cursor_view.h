#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_GRABBY_CURSOR_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_GRABBY_CURSOR_VIEW_H_

#include <gtk/gtk.h>
#include <better_c_std/result.h>

typedef struct {
    GtkWidget* widget;
} MgGrabbyCursorView;

typedef STRUCT_RESULT(MgGrabbyCursorView*, GError*) MgGrabbyCursorViewResult;

MgGrabbyCursorViewResult MgGrabbyCursorView_create(GtkBuilder* ui, const char* widget_name);
MgGrabbyCursorView*    MgGrabbyCursorView_from_widget(GtkWidget* widget);

void MgGrabbyCursorView_free(MgGrabbyCursorView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_GRABBY_CURSOR_VIEW_H_