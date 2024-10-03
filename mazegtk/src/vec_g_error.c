#include <mazegtk/vec_g_error.h>

static void GError_ptr_free(GError_ptr ptr) {
    if (ptr)
        g_error_free(ptr);
}

#define VECTOR_C GError_ptr
#define VECTOR_ITEM_DESTRUCTOR GError_ptr_free
#include <better_c_std/vector.h>
