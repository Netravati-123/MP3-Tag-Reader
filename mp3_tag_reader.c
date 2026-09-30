#include <stdio.h>
#include <string.h>
#include<stdlib.h>
#include "mp3_tag_reader.h"

Status check_mp3_extension(char *filename)
{
    char *extension;

    extension = strrchr(filename, '.');

    if(extension != NULL && strcmp(extension, ".mp3") == 0)
    {
        return e_success;
    }

    return e_failure;
}


Status open_mp3_file(Mp3TagReader *mp3, char *filename)
{
    mp3->fptr_mp3 = fopen(filename, "rb");

    if(mp3->fptr_mp3 == NULL)
    {
        return e_failure;
    }

    return e_success;
}


Status check_id3_signature(Mp3TagReader *mp3)
{
    char buffer[4];

    if(fread(buffer, 1, 3, mp3->fptr_mp3) != 3)
    {
        return e_failure;
    }

    buffer[3] = '\0';

    if(strcmp(buffer, "ID3") == 0)
    {
        return e_success;
    }

    return e_failure;
}


Status read_tags(Mp3TagReader *mp3)
{
    char frame_id[5];
    unsigned char size_buffer[4];
    unsigned char flag_buffer[2];
    unsigned int frame_size;
    char *data;

    /* Move file pointer to byte 10 */
    fseek(mp3->fptr_mp3, 10, SEEK_SET);

    while(1)
    {
        /* Read frame ID - 4 bytes */
        if(fread(frame_id, 1, 4, mp3->fptr_mp3) != 4)
        {
            break;
        }

        frame_id[4] = '\0';

        /* Read frame size - 4 bytes */
        if(fread(size_buffer, 1, 4, mp3->fptr_mp3) != 4)
        {
            break;
        }

        /* Convert big-endian size */
        frame_size = ((unsigned int)size_buffer[0] << 24) |
                     ((unsigned int)size_buffer[1] << 16) |
                     ((unsigned int)size_buffer[2] << 8)  |
                     ((unsigned int)size_buffer[3]);

        /* Read 2 bytes of flags */
        if(fread(flag_buffer, 1, 2, mp3->fptr_mp3) != 2)
        {
            break;
        }

        /* If frame size is 0, stop */
        if(frame_size == 0)
        {
            break;
        }

        /* Allocate memory for frame data */
        data = malloc(frame_size + 1);

        if(data == NULL)
        {
            return e_failure;
        }

        /* Read frame data */
        if(fread(data, 1, frame_size, mp3->fptr_mp3) != frame_size)
        {
            free(data);
            break;
        }

        data[frame_size] = '\0';

        /*
         * Check only required frame IDs
         */

        if(strcmp(frame_id, "TIT2") == 0)
        {
            printf("Title : %s\n", data + 1);
        }
        else if(strcmp(frame_id, "TPE1") == 0)
        {
            printf("Artist : %s\n", data + 1);
        }
        else if(strcmp(frame_id, "TALB") == 0)
        {
            printf("Album : %s\n", data + 1);
        }
        else if(strcmp(frame_id, "TYER") == 0)
        {
            printf("Year : %s\n", data + 1);
        }
        else if(strcmp(frame_id, "TCON") == 0)
        {
            printf("Genre : %s\n", data + 1);
        }
        else if(strcmp(frame_id, "COMM") == 0)
        {
            printf("Comment : %s\n", data + 1);
        }

        /* Free allocated memory */
        free(data);
    }

    return e_success;
}