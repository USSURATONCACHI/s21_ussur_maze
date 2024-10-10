#ifndef LIBMAZE_ERROR_H_
#define LIBMAZE_ERROR_H_

#include <better_c_std/result.h>

typedef enum {
    MZ_ERROR_NOT_ENOUGH_MEMORY,
    MZ_ERROR_THREAD_CRASHED,
} MzError;

typedef struct {} MzVoid;
typedef STRUCT_RESULT(MzVoid, MzError) MzVoidResult;

#endif // LIBMAZE_ERROR_H_