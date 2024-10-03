#include <mazegtk/vec_g_error.h>

#define VECTOR_C GError_ptr
#define VECTOR_ITEM_DESTRUCTOR g_error_free
#include <better_c_std/vector.h>
