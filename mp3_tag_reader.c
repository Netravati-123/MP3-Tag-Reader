#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

    fseek(mp3->fptr_mp3, 10, SEEK_SET);

    while(1)
    {
        if(fread(frame_id, 1, 4, mp3->fptr_mp3) != 4)
        {
            break;
        }

        frame_id[4] = '\0';

        if(frame_id[0] == '\0')
        {
            break;
        }

        if(fread(size_buffer, 1, 4, mp3->fptr_mp3) != 4)
        {
            break;
        }

        frame_size = ((unsigned int)size_buffer[0] << 24) |
                     ((unsigned int)size_buffer[1] << 16) |
                     ((unsigned int)size_buffer[2] << 8) |
                     ((unsigned int)size_buffer[3]);

        if(frame_size == 0)
        {
            break;
        }

        if(fread(flag_buffer, 1, 2, mp3->fptr_mp3) != 2)
        {
            break;
        }

        data = malloc(frame_size + 1);

        if(data == NULL)
        {
            return e_failure;
        }

        if(fread(data, 1, frame_size, mp3->fptr_mp3) != frame_size)
        {
            free(data);
            break;
        }

        data[frame_size] = '\0';

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

        free(data);
    }

    return e_success;
}

Status edit_tag(Mp3TagReader *mp3, char *tag, char *new_data)
{
    char frame_id[5];
    unsigned char size_buffer[4];
    unsigned char flag_buffer[2];

    unsigned int frame_size;
    unsigned int new_size;

    char *data;

    char header[10];
    char buffer[1024];

    int bytes_read;

    printf("Inside edit_tag\n");

    mp3->fptr_temp = fopen("temp.mp3", "wb");

    if(mp3->fptr_temp == NULL)
    {
        printf("1. Failed to open temp.mp3\n");
        return e_failure;
    }

    printf("1. temp.mp3 opened\n");

    fseek(mp3->fptr_mp3, 0, SEEK_SET);

    if(fread(header, 1, 10, mp3->fptr_mp3) != 10)
    {
        printf("2. Failed to read MP3 header\n");
        fclose(mp3->fptr_temp);
        return e_failure;
    }

    printf("2. MP3 header read successfully\n");

    fwrite(header, 1, 10, mp3->fptr_temp);

    fseek(mp3->fptr_mp3, 10, SEEK_SET);

    printf("3. Starting frame search\n");

    while(1)
    {
        if(fread(frame_id, 1, 4, mp3->fptr_mp3) != 4)
        {
            printf("Could not read frame ID\n");
            break;
        }

        frame_id[4] = '\0';

        if(frame_id[0] == '\0')
        {
            printf("End of frames\n");
            break;
        }

        printf("Frame found : %s\n", frame_id);

        if(fread(size_buffer, 1, 4, mp3->fptr_mp3) != 4)
        {
            printf("Could not read frame size\n");
            break;
        }

        frame_size = ((unsigned int)size_buffer[0] << 24) |
                     ((unsigned int)size_buffer[1] << 16) |
                     ((unsigned int)size_buffer[2] << 8) |
                     ((unsigned int)size_buffer[3]);

        if(fread(flag_buffer, 1, 2, mp3->fptr_mp3) != 2)
        {
            printf("Could not read frame flags\n");
            break;
        }

        if(strcmp(frame_id, tag) == 0)
        {
            printf("Matching tag found : %s\n", frame_id);

            new_size = strlen(new_data) + 1;

            data = malloc(new_size);

            if(data == NULL)
            {
                printf("Memory allocation failed\n");
                fclose(mp3->fptr_temp);
                return e_failure;
            }

            data[0] = 0;
            strcpy(data + 1, new_data);

            fwrite(frame_id, 1, 4, mp3->fptr_temp);

            unsigned char new_size_buffer[4];

            new_size_buffer[0] = (new_size >> 24) & 0xFF;
            new_size_buffer[1] = (new_size >> 16) & 0xFF;
            new_size_buffer[2] = (new_size >> 8) & 0xFF;
            new_size_buffer[3] = new_size & 0xFF;

            fwrite(new_size_buffer, 1, 4, mp3->fptr_temp);

            fwrite(flag_buffer, 1, 2, mp3->fptr_temp);

            fwrite(data, 1, new_size, mp3->fptr_temp);

            free(data);

            fseek(mp3->fptr_mp3, frame_size, SEEK_CUR);

            printf("Tag updated in temporary file\n");
        }
        else
        {
            fwrite(frame_id, 1, 4, mp3->fptr_temp);
            fwrite(size_buffer, 1, 4, mp3->fptr_temp);
            fwrite(flag_buffer, 1, 2, mp3->fptr_temp);

            data = malloc(frame_size);

            if(data == NULL)
            {
                printf("Memory allocation failed\n");
                fclose(mp3->fptr_temp);
                return e_failure;
            }

            if(fread(data, 1, frame_size, mp3->fptr_mp3) != frame_size)
            {
                printf("Could not read frame data\n");
                free(data);
                break;
            }

            fwrite(data, 1, frame_size, mp3->fptr_temp);

            free(data);
        }
    }

    printf("4. Copying remaining MP3 data\n");

    while((bytes_read = fread(buffer, 1, sizeof(buffer),
                              mp3->fptr_mp3)) > 0)
    {
        fwrite(buffer, 1, bytes_read, mp3->fptr_temp);
    }

    fclose(mp3->fptr_mp3);
    fclose(mp3->fptr_temp);

  /** remove("sample.mp3");

if(rename("temp.mp3", "sample.mp3") != 0)
{
    printf("ERROR: Unable to replace original file\n");
    return e_failure;
}

printf("Original file updated successfully\n");*/

    printf("5. Temporary file created successfully\n");

    return e_success;
}