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
    FILE *fptr_temp;
} Mp3TagReader;

/*typedef struct
{
    char *tag;
    char *new_data;
}EditInfo;*/

#endif