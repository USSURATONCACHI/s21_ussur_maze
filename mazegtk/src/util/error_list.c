#include <mazegtk/util/error_list.h>
#include <mazegtk/util/domain.h>

#include <better_c_std/string/str_t.h>
#include <better_c_std/prettify.h>

GError** error_list_get_nullptr(vec_GError_ptr* list) {
    if (list->length == 0) {
        vec_GError_ptr_push(list, NULL);
        return &list->data[0];
    }

    GError** last_elem = &list->data[list->length - 1];
    if (*last_elem == NULL) {
        return last_elem;
    } else {
        vec_GError_ptr_push(list, NULL);
        return &list->data[list->length - 1];
    }
}

size_t error_list_errors_count(const vec_GError_ptr* list) {
    if (list->length == 0)
        return 0;

    GError* last_elem = list->data[list->length - 1];
    if (last_elem == NULL)
        return list->length - 1;
    else
        return list->length;
}

void error_list_print_errors(const vec_GError_ptr* list) {
    int err_count = error_list_errors_count(list);
    debugln("Encountered %d error(s):", err_count);
    debug_push();
    for (int i = 0; i < err_count; i++) {
        const GError* error = list->data[i];
        if (error == NULL)
            continue;
        debugln("[%s][%d]: %s", g_quark_to_string(error->domain), error->code, error->message);
    }
    debug_pop();
}

void error_list_first_to_g_error(const vec_GError_ptr* list, str_t prefix, GError** out_error) {
    int err_count = error_list_errors_count(list);
    GError* error = list->data[0];
    if (err_count > 0 && out_error)
        *out_error = g_error_new(DOMAIN, 1, 
            "%s. Total errors: %d: [%s][%d] %s",
            prefix.string,
            err_count,
            g_quark_to_string(error->domain), 
            error->code, 
            error->message
        );
    
    str_free(prefix);
}