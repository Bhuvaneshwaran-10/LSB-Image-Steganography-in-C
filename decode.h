#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_FILE_SUFFIX 32

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Output File Info */
    char output_fname[100];
    FILE *fptr_output;
    int extn_size;
    char extn_name[100];
    char extn_secret_file[MAX_FILE_SUFFIX];
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Magic String */
    int magic_size;
    char magic_string[100];

}DecodeInfo;

/* Decoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Store Magic String Length*/
Status decode_magic_string_len(int *magic_size, DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(int magic_size,DecodeInfo *decInfo);

/* Decode secret file extenstion length*/
Status decode_secret_file_extn_len(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* open new extention file */
Status open_updated_file( DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode a LSB into byte of image data array */
char decode_lsb_to_byte(char *image_buffer);

/* Decode a LSB into size of image data array */
int decode_lsb_to_size(char *image_buffer);

#endif