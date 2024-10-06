#ifndef MAZEGTK_UTIL_RESULT_H_
#define MAZEGTK_UTIL_RESULT_H_

#define STRUCT_RESULT(Ok, Err) \
    struct {           \
        bool is_ok;    \
        union {        \
            Ok ok;     \
            Err error; \
        };             \
    }

#define OK(ok_value)   { .is_ok = true,  .ok    = ok_value  }
#define ERR(err_value) { .is_ok = false, .error = err_value }

#endif // MAZEGTK_UTIL_RESULT_H_