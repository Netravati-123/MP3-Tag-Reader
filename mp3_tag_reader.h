#ifndef MP3_TAG_READER_H
#define MP3_TAG_READER_H

#include "types.h"

Status check_mp3_extension(char *filename);
Status open_mp3_file(Mp3TagReader *mp3, char *filename);
Status check_id3_signature(Mp3TagReader *mp3);
Status read_tags(Mp3TagReader *mp3);


#endif