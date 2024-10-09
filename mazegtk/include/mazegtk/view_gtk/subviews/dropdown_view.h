#ifndef MAZEGTK_VIEW_GTK_SUBVIEWS_DROPDOWN_VIEW_H_
#define MAZEGTK_VIEW_GTK_SUBVIEWS_DROPDOWN_VIEW_H_

#include <mazegtk/controller/controller.h>
#include <better_c_std/result.h>
#include <gtk/gtk.h>

typedef struct {
    MgController* controller;

    GtkWindow* window;
    GtkWidget* dropdown_ui;
    GtkStack*  main_stack;

    GtkWidget* last_visible_child;
} MgDropdownView;

typedef STRUCT_RESULT(MgDropdownView*, GError*) MgDropdownViewResult;

MgDropdownViewResult MgDropdownView_create(GtkBuilder* ui, MgController* controller);
void MgDropdownView_free(MgDropdownView* view);

#endif // MAZEGTK_VIEW_GTK_SUBVIEWS_DROPDOWN_VIEW_H_