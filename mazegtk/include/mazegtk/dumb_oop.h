#ifndef MAZEGTK_DUMB_OOP_H_
#define MAZEGTK_DUMB_OOP_H_

#define METHOD(ret, name, ...) ret (*name)(void* this, __VA_ARGS__);
#define PUBLIC(x) x
#define PRIVATE(x) struct { x } private;

#define ST_METHOD_NAME(className, name) className##_##name
#define ST_METHOD(className, ret, name, ...) ret className##_##name (__VA_ARGS__)

#endif // MAZEGTK_DUMB_OOP_H_