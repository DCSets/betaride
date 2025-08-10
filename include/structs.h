#include "Arduino.h"

#ifndef STRUCTS_H
#define STRUCTS_H

struct Resource
{
    char id[32];
    char name[64];
    char type[8];
    bool isNew;
};

#endif