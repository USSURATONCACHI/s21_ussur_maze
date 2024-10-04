#ifndef MAZEGTK_UTIL_COMMON_MACROS_H_
#define MAZEGTK_UTIL_COMMON_MACROS_H_

#include <gtk/gtk.h>

#define DOMAIN g_quark_from_static_string("s21_ussur_mazegtk")

#define GETREF(variable, type, builder, name, out_error)                                 \
    variable = type(gtk_builder_get_object((builder), (name)));                          \
    if (variable == NULL && out_error) {                                                 \
        *(out_error) = g_error_new(DOMAIN, 2, "Did not find widget %s in builder", name);\
    }

#define PUTERR(out_error) if (out_error) (*(out_error))


#endif // MAZEGTK_UTIL_COMMON_MACROS_H_