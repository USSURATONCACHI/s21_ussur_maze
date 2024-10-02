#ifndef MAZEGTK_DUMB_OOP_H_
#define MAZEGTK_DUMB_OOP_H_

#define PUBLIC(x) x
#define PRIVATE(x) struct { x } private;

#define METHOD(ret, name, ...) ret (*name)(void* this, __VA_ARGS__);
#define METHODP(ret, name) ret (*name)(void* this);

#define METHOD_NAME(className, name) className##_##name
#define IMPL_METHOD(className, ret, name, ...) ret className##_##name(className* this, __VA_ARGS__)
#define IMPL_METHODP(className, ret, name)     ret className##_##name(className* this)

#define ST_METHOD(className, ret, name, ...) ret className##_##name (__VA_ARGS__)

#endif // MAZEGTK_DUMB_OOP_H_