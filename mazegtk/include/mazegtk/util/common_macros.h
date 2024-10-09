#ifndef MAZEGTK_UTIL_COMMON_MACROS_H_
#define MAZEGTK_UTIL_COMMON_MACROS_H_

#include <gtk/gtk.h>
#include <better_c_std/prettify/misc.h>

#define GETREF(variable, type, builder, name, out_error)                                 \
    variable = type(gtk_builder_get_object((builder), (name)));                          \
    if (variable == NULL && out_error) {                                                 \
        *(out_error) = g_error_new(DOMAIN, 2, "Did not find widget %s in builder", name);\
    }

#define PUTERR(out_error) if (out_error) (*(out_error))

#define GERROR_NEW(...) g_error_new(g_quark_from_string(__FILE__ ":" STR(__LINE__)), 1, __VA_ARGS__)

#define G_UNREF_SAFE(ptr) { if (ptr) g_object_unref(ptr); }

#endif // MAZEGTK_UTIL_COMMON_MACROS_H_