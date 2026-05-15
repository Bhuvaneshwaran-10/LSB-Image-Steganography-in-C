#include <stdio.h>
#include <string.h>
#include "types.h"
#include "decode.h"
#include "common.h"

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strstr(argv[2], ".bmp") != NULL)
    {
        printf(".bmp file is present\n");
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf(".bmp is not present\n");
        return d_failure;
    }

    // user gave output filename, copy it
    if (argv[3] != NULL)
    {
        strcpy(decInfo->output_fname, argv[3]);
    }
    else
    {
        strcpy(decInfo->output_fname, "output");
    }
    return d_success;
}

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    if (decInfo->fptr_stego_image == NULL)
    {
        printf("ERROR: Unable to open file\n");
        return d_failure;
    }
    return d_success;
}

/* Decode a LSB into byte of image data array */
char decode_lsb_to_byte(char *image_buffer)
{
    unsigned char byte = 0;
    for (int i = 0; i < 8; i++)
    {
        byte = byte << 1;
        int bit = image_buffer[i] & 1;
        byte = byte | bit;
    }
    return byte;
}

/* Decode a LSB into size of image data array */
int decode_lsb_to_size(char *image_buffer)
{
    unsigned int size = 0;   
    for (int i = 0; i < 32; i++)
    {
        size = size << 1;
        int bit = image_buffer[i] & 1;
        size = size | bit;
    }
    return size;
}

/* Store Magic String Length*/
Status decode_magic_string_len(int *magic_size, DecodeInfo *decInfo)
{
    unsigned char buffer[32];
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        printf("Unable to read magic string length\n");
        return d_failure;
    }
    *magic_size = decode_lsb_to_size(buffer);
    return d_success;
}

/* Store Magic String */
Status decode_magic_string(int magic_size, DecodeInfo *decInfo)
{
    unsigned char buffer[8];
    if (magic_size > 99)
    {
        printf("Invalid magic size\n");
        return d_failure;
    }

    char magic[100];

    for (int i = 0; i < magic_size; i++)
    {
        fread(buffer, 8, 1, decInfo->fptr_stego_image);
        magic[i] = decode_lsb_to_byte(buffer);
    }
    magic[magic_size] = '\0';

    if (strcmp(magic, decInfo->magic_string) != 0)
    {
        printf("ERROR: Magic string mismatch! Wrong passkey.\n");
        return d_failure;
    }
    printf("Magic string verification successful..\n");
    return d_success;
}

/* Decode secret file extenstion length*/
Status decode_secret_file_extn_len(DecodeInfo *decInfo)
{
    unsigned char buffer[32];
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        printf("Unable to read extension size\n");
        return d_failure;
    }
    decInfo->extn_size = 0;
    for (int i = 0; i < 32; i++)
    {
        decInfo->extn_size = (decInfo->extn_size << 1) | (buffer[i] & 1);
    }
    return d_success;
}

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    unsigned char buffer[8];
    for (int i = 0; i < decInfo->extn_size; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_stego_image);
        decInfo->extn_secret_file[i] = decode_lsb_to_byte(buffer);
    }
    decInfo->extn_secret_file[decInfo->extn_size] = '\0';
    return d_success;
}

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned char buffer[32];
    if (fread(buffer, 1, 32, decInfo->fptr_stego_image) != 32)
    {
        printf("Unable to read file size\n");
        return d_failure;
    }
    decInfo->size_secret_file = 0;
    for (int i = 0; i < 32; i++)
    {
        decInfo->size_secret_file = (decInfo->size_secret_file << 1) | (buffer[i] & 1);
    }
    return d_success;
}

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    unsigned char buffer[8];
    for (long i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(buffer, 1, 8, decInfo->fptr_stego_image) != 8)
        {
            printf("Unable to read secret data from image\n");
            return d_failure;
        }
        char ch = decode_lsb_to_byte(buffer);
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }
    return d_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    if (open_decode_files(decInfo) == d_failure)
    {
        printf("Open file is failed\n");
        return d_failure;
    }
    printf("Open files successfully..\n");

    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    if (decode_magic_string_len(&decInfo->magic_size, decInfo) == d_failure)
    {
        printf("Magic string length is failed\n");
        return d_failure;
    }

    if (decode_magic_string(decInfo->magic_size, decInfo) == d_failure)
    {
        printf("Magic string is Failed\n");
        return d_failure;
    }
    printf("Magic String is successful..\n");

    if (decode_secret_file_extn_len(decInfo) == d_failure)
    {
        printf("Secret file exten length is failed\n");
        return d_failure;
    }

    if (decode_secret_file_extn(decInfo) == d_failure)
    {
        printf("Secret file Extension is Failed\n");
        return d_failure;
    }
    printf("Secret file Extension is successful..\n");

    strcpy(decInfo->extn_name,decInfo->output_fname);
    if(decInfo->extn_size > 0)
    {
        strcat(decInfo->extn_name,decInfo->extn_secret_file);
    }
    decInfo->fptr_output = fopen(decInfo->extn_name, "w");
    if (decInfo->fptr_output == NULL)
    {
        printf("Cannot create output file\n");
        return d_failure;
    }

    if (decode_secret_file_size(decInfo) == d_failure)
    {
        printf("Secret file Size is Failed\n");  
        return d_failure;
    }
    printf("Secret file Size is successful..\n");

    if (decode_secret_file_data(decInfo) == d_failure)
    {
        printf("Secret file Data is Failed\n");
        return d_failure;
    }
    printf("Secret file Data is successful..\n");
    return d_success;
}
