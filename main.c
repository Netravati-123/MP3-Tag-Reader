#include <stdio.h>
#include <string.h>
#include "mp3_tag_reader.h"

int main(int argc, char *argv[])
{
    Mp3TagReader mp3;

    if(argc != 3)
    {
        printf("Usage: ./a.out -v <file.mp3>\n");
        return 0;
    }

    /* Check operation type */
    if(strcmp(argv[1], "-v") == 0)
    {
        /* Check .mp3 extension */
        if(check_mp3_extension(argv[2]) == e_failure)
        {
            printf("ERROR: File should have .mp3 extension\n");
            return 0;
        }

        /* Open MP3 file */
        if(open_mp3_file(&mp3, argv[2]) == e_failure)
        {
            printf("ERROR: Unable to open MP3 file\n");
            return 0;
        }

        /* Check ID3 signature */
        if(check_id3_signature(&mp3) == e_failure)
        {
            printf("ERROR: ID3 signature not found\n");
            fclose(mp3.fptr_mp3);
            return 0;
        }

        printf("Valid MP3 file\n");
        printf("ID3 signature found\n");

        /* Read tags */
        if(read_tags(&mp3) == e_failure)
        {
            printf("ERROR: Unable to read tags\n");
            fclose(mp3.fptr_mp3);
            return 0;
        }

        fclose(mp3.fptr_mp3);
    }
    else
    {
        printf("ERROR: Invalid operation\n");
        printf("Use -v for viewing MP3 tags\n");
    }

    return 0;
}