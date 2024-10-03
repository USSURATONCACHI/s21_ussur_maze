#ifndef MAZEGTK_VEC_G_ERROR_H_
#define MAZEGTK_VEC_G_ERROR_H_

#include <glib.h>

typedef GError* GError_ptr;
#define VECTOR_ITEM_TYPE GError_ptr
#define VECTOR_ITEM_DESTRUCTOR g_error_free
#include <better_c_std/vector.h>


#endif // MAZEGTK_VEC_G_ERROR_H_