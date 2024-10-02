#ifndef LIBMAZE_UTIL_H_
#define LIBMAZE_UTIL_H_

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#define unreachable() __builtin_unreachable()

#define panic(...)                                                          \
  {                                                                         \
    printf("\n+-+-+-+-+-+-+-+-+-+-\n");                      \
    printf("Panic in file %s:%d, function %s: \n", __FILE__, \
              __LINE__, __func__);                                          \
    printf(__VA_ARGS__);                                     \
    printf("\n");                                            \
    exit(-1);                                                               \
  }

#define forget(value)
#define unused(var) var = var

#define debugln(...)                                           \
  {                                                            \
    printf("LOG (%s:%d): ", __FILE__, __LINE__); \
    printf(__VA_ARGS__);                         \
    printf("\n");                                \
  }
#define debug(...)                                             \
  {                                                            \
    printf("LOG (%s:%d): ", __FILE__, __LINE__); \
    printf(__VA_ARGS__);                         \
  }
#define debugc(...) \
  { printf(__VA_ARGS__); }

#define assert_alloc(ptr) \
  if (ptr == NULL) panic("Failed to allocate memory");

#define assert_m(cond) \
  if (not(cond)) panic("Assertion failed\n");

#define CONCAT2(s1, s2) s1##s2
#define CONCAT(s1, s2) CONCAT2(s1, s2)

#define STR2(s) #s
#define STR(s) STR2(s)

#define LEN(array) (sizeof(array) / sizeof(array[0]))

#define SWAP(type, a, b) \
  {                      \
    type tmp = (a);      \
    (a) = (b);           \
    (b) = tmp;           \
  }

#define foreach_extract(item, vec, condition, code)                           \
  {                                                                           \
    int i;                                                                    \
    for (i = 0; i < (vec).length and (condition); i++) {                      \
      item = (vec).data[i];                                                   \
      code;                                                                   \
    }                                                                         \
                                                                              \
    (vec).length -= i;                                                        \
    for (int j = 0; j < (vec).length; j++) (vec).data[j] = (vec).data[j + i]; \
  }



#endif // LIBMAZE_UTIL_H_