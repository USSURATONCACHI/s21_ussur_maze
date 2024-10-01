#ifndef MAZEGTK_MODEL_H_
#define MAZEGTK_MODEL_H_

#include <mazegtk/dumb_oop.h>

// Class MazegtkModel
#define Self MazegtkModel

typedef struct {
    PUBLIC(

    )

    PRIVATE(

    )

    
} MazegtkModel;

ST_METHOD(MazegtkModel, Self, create);
ST_METHOD(MazegtkModel, void, free, MazegtkModel);

#undef Self

#endif // MAZEGTK_MODEL_H_