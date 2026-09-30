#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

typedef enum
{
    e_success,
    e_failure
} Status;

typedef struct
{
    FILE *fptr_mp3;
} Mp3TagReader;


#endif