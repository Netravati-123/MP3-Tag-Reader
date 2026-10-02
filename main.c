#include <stdio.h>
#include <string.h>
#include "mp3_tag_reader.h"

int main(int argc, char *argv[])
{
    Mp3TagReader mp3;
    char tag[5];

    if(argc < 2)
    {
        printf("ERROR: Insufficient arguments\n");
        return 0;
    }

    /* View operation */
    if(strcmp(argv[1], "-v") == 0)
    {
        if(argc != 3)
        {
            printf("Usage: ./a.out -v <file.mp3>\n");
            return 0;
        }

        if(check_mp3_extension(argv[2]) == e_failure)
        {
            printf("ERROR: File should have .mp3 extension\n");
            return 0;
        }

        if(open_mp3_file(&mp3, argv[2]) == e_failure)
        {
            printf("ERROR: Unable to open MP3 file\n");
            return 0;
        }

        if(check_id3_signature(&mp3) == e_failure)
        {
            printf("ERROR: ID3 signature not found\n");
            fclose(mp3.fptr_mp3);
            return 0;
        }

        printf("Valid MP3 file\n");
        printf("ID3 signature found\n");

        read_tags(&mp3);

        fclose(mp3.fptr_mp3);
    }

    /* Edit operation */
    else if(strcmp(argv[1], "-e") == 0)
    {
        if(argc != 5)
        {
            printf("Usage: ./a.out -e -t/-a/-A/-y/-g/-c <data> <file.mp3>\n");
            return 0;
        }

        if(check_mp3_extension(argv[4]) == e_failure)
        {
            printf("ERROR: File should have .mp3 extension\n");
            return 0;
        }

        if(strcmp(argv[2], "-t") != 0 &&
           strcmp(argv[2], "-a") != 0 &&
           strcmp(argv[2], "-A") != 0 &&
           strcmp(argv[2], "-y") != 0 &&
           strcmp(argv[2], "-g") != 0 &&
           strcmp(argv[2], "-c") != 0)
        {
            printf("ERROR: Invalid tag option\n");
            return 0;
        }

        if(strcmp(argv[2], "-t") == 0)
        {
            strcpy(tag, "TIT2");
        }
        else if(strcmp(argv[2], "-a") == 0)
        {
            strcpy(tag, "TPE1");
        }
        else if(strcmp(argv[2], "-A") == 0)
        {
            strcpy(tag, "TALB");
        }
        else if(strcmp(argv[2], "-y") == 0)
        {
            strcpy(tag, "TYER");
        }
        else if(strcmp(argv[2], "-g") == 0)
        {
            strcpy(tag, "TCON");
        }
        else
        {
            strcpy(tag, "COMM");
        }

        if(open_mp3_file(&mp3, argv[4]) == e_failure)
        {
            printf("ERROR: Unable to open MP3 file\n");
            return 0;
        }

        if(check_id3_signature(&mp3) == e_failure)
        {
            printf("ERROR: ID3 signature not found\n");
            fclose(mp3.fptr_mp3);
            return 0;
        }

        printf("Valid MP3 file\n");
        printf("ID3 signature found\n");

        printf("Tag to edit : %s\n", tag);
        printf("New data    : %s\n", argv[3]);

        if(edit_tag(&mp3, tag, argv[3]) == e_failure)
        {
            printf("ERROR: Unable to edit tag\n");
            return 0;
        }

        printf("Tag edited successfully\n");
    }

    else
    {
        printf("ERROR: Invalid operation\n");
        printf("Use -v for view or -e for edit\n");
    }

    return 0;
}