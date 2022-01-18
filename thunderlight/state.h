#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "duostate.h"


typedef struct {
    PyObject_HEAD
    Duostate *duostate;
} State;

extern PyTypeObject StateType;

PyObject *State_new(Duostate *duostate);

#ifdef __cplusplus
}
#endif
