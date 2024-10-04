#ifndef MAZEGTK_UTIL_GETREF_H_
#define MAZEGTK_UTIL_GETREF_H_

#include <mazegtk/util/domain.h>

#define GETREF(variable, type, builder, name, out_error)                                 \
    variable = type(gtk_builder_get_object((builder), (name)));                          \
    if (variable == NULL && out_error) {                                                 \
        *(out_error) = g_error_new(DOMAIN, 2, "Did not find widget %s in builder", name);\
    }

#endif // MAZEGTK_UTIL_GETREF_H_