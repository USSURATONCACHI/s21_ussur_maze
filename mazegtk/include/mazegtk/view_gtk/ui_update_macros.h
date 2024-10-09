#ifndef MAZEGTK_VIEW_GTK_UI_UPDATE_MACROS_H_
#define MAZEGTK_VIEW_GTK_UI_UPDATE_MACROS_H_

#define UPDATE_SPINBTN_GDOUBLE(updating_btn, last_shown_var, new_value) \
    {                                                                                   \
        if (updating_btn && !gtk_widget_is_focus(GTK_WIDGET(updating_btn))) {           \
            (last_shown_var) = (new_value);                                             \
            gtk_spin_button_set_value(GTK_SPIN_BUTTON(updating_btn), (last_shown_var)); \
        }                                                                               \
    }                                                                                   \
    //;

#define CHECK_GDOUBLE_CHANGED(varname, widget, check_against) \
    gdouble (varname) = gtk_spin_button_get_value(widget);    \
    if ((varname) == (check_against))                         \
        return;                                               \
    (check_against) = val;                                    \
    // ;

#endif // MAZEGTK_VIEW_GTK_UI_UPDATE_MACROS_H_