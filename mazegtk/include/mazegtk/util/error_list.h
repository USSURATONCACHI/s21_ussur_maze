#ifndef MAZEGTK_UTIL_ERROR_LIST_H_
#define MAZEGTK_UTIL_ERROR_LIST_H_

#include <mazegtk/util/vec_g_error.h>
#include <better_c_std/string/str_t_raw.h>

GError** error_list_get_nullptr(vec_GError_ptr* list);
size_t error_list_errors_count(const vec_GError_ptr* list);
void error_list_print_errors(const vec_GError_ptr* list);
void error_list_first_to_g_error(const vec_GError_ptr* list, str_t prefix, GError** out_error);

#endif // MAZEGTK_UTIL_ERROR_LIST_H_