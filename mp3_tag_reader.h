#ifndef MP3_TAG_READER_H
#define MP3_TAG_READER_H

#include "types.h"

/* Validate .mp3 extension */
Status check_mp3_extension(char *filename);

/* Open MP3 file */
Status open_mp3_file(Mp3TagReader *mp3, char *filename);

/* Check ID3 signature */
Status check_id3_signature(Mp3TagReader *mp3);

/* Read and display tags */
Status read_tags(Mp3TagReader *mp3);

/* Edit selected tag */
Status edit_tag(Mp3TagReader *mp3,char *tag, char *new_data);

#endif