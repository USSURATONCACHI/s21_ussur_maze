#include <mazegtk/view/gtk_view.h>

bool MgGtkView_is_fine(const MgGtkView* view) {
    return !view->is_stopped;
}