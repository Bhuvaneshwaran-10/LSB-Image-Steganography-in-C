
#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include"common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //argv[2] -> .bmp -> store argv[2] in structure encInfo->src_image_fname = argv[2];
        if(argv[2] != NULL &&  strcmp(strstr(argv[2],"."), ".bmp") == 0)
        {
            encInfo -> src_image_fname = argv[2];
        }
        else
        {
            return e_failure;
        }
        //argv[3] -> .txt -> encInfo->secret_fname = argv[3] -> strcpy(encInfo->extn_secret_file, strstr(argv[3], "."))
        if(argv[3] != NULL && strcmp(strstr(argv[3],"."), ".txt") == 0)
        {
            encInfo -> secret_fname = argv[3]; 
        }
        else
        {
            return e_failure;
        }
            //argv[4] -> NULL-> store "stego.bmp" in structure
            //   -> no NULL -> .bmp -> store argv[4] in structure
        if(argv[4] != NULL)
        {
            encInfo -> stego_image_fname = argv[4];
        }
        else
        {
            encInfo -> stego_image_fname = "steago.bmp";
        }

    return e_success;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

/* Check capacity*/
Status check_capacity(EncodeInfo *encInfo)
{

    int image_size = get_image_size_for_bmp(encInfo->fptr_src_image);

    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    char *ptr = strrchr(encInfo->secret_fname,'.');
    if(ptr == NULL)
        return e_failure;
    strcpy(encInfo->extn_secret_file,ptr);

    int bytes_to_encode = ((strlen(encInfo->magic_string) + sizeof(int) + strlen(encInfo->extn_secret_file) + sizeof(int) + encInfo->size_secret_file )*8);
    if((image_size) >= bytes_to_encode){
        return e_success;
     }
    else
    {
        return e_failure;
    }
}

/* Get image size*/
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    
    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);


    printf("---------------------------------------------\n");
    printf("height = %u\n", height);
    printf("width = %u\n", width);
    printf("Image capacity = %u\n", (width*height*3));
    printf("---------------------------------------------\n");

    // Return image capacity
    return width * height * 3;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0,SEEK_END);
    uint size = ftell(fptr);
    rewind(fptr);
    return size;
}


/* Encode copy bmp image header*/
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char header[54];
    rewind(fptr_src_image);
    fread(header,54,1,fptr_src_image);
    fwrite(header,54,1,fptr_dest_image);
    return e_success;
}

/* Encode magic string length */
Status encode_magic_string_len(int len, EncodeInfo *encInfo)
{      
    unsigned char buffer[32];

    if (fread(buffer, 1, 32, encInfo->fptr_src_image) != 32)
        return e_failure;

    encode_size_to_lsb(len, buffer);

    fwrite(buffer, 1, 32, encInfo->fptr_stego_image);

    return e_success;
}

/* Encode magic string*/
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    return encode_data_to_image((char *)magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
}

/* Encode secret file extension size */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    unsigned char buffer[8];
    for(int i=0;i<strlen(file_extn);i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success; 
}

/* Encode secret file extension length */
Status encode_secret_file_extn_len(int len, EncodeInfo *encInfo)
{
    unsigned char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(len,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

/* Encode Secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
     unsigned char buffer[32];
     fread(buffer,32,1,encInfo->fptr_src_image);
     encode_size_to_lsb(file_size,buffer);
     fwrite(buffer,32,1,encInfo->fptr_stego_image);
     return e_success;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    unsigned char buffer[8];
    unsigned char ch;
    for(int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(&ch,1,1,encInfo->fptr_secret);
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(ch,buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[8];
    for(int i = 0; i < size; i++)
    {
        fread(arr, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], arr);
        fwrite(arr, 8, 1, fptr_stego_image);
    }
    return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_steago_image)
{
    unsigned char ch;
    while (fread(&ch,1,1,fptr_src_image))
    {
       fwrite(&ch,1,1,fptr_steago_image);
    }
    return e_success;
}

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(unsigned char data,unsigned char *image_buffer)
{
    for(int i = 0; i < 8; i++)
    {
        int bit = ((data>>(7-i))&1);
        image_buffer[i] = image_buffer[i] & 0XFE;
        image_buffer[i] = image_buffer[i] | bit;
    }    
    return e_success;
}

/* Encode a size into LSB of image data array */
Status encode_size_to_lsb(unsigned int data, unsigned char *image_buffer)
{
    for(int i = 0 ; i < 32 ; i++)
    {
        int bit = ((data >> (31 - i)) & 1);
        image_buffer[i] = image_buffer[i] & 0xFE;
        image_buffer[i] = image_buffer[i] | bit;
    }
    return e_success;
}

/* Perform enconding*/
Status do_encoding(EncodeInfo *encInfo)
{
     //open_files() == e_failure -> error -> return e_failure
    
    //check_capacity() == e_failure -> error -> return e_failure

    //copy_bmp_header() == e_failure -> error -> return e_failure

    //encode_magic_string() == e_failure -> error -> return e_failure

    if(open_files(encInfo) == e_failure)
    {
        printf("Open files failed..\n");
        return e_failure;
    }
    printf("Files opened successfully..\n");

    if(check_capacity(encInfo) == e_failure)
    {
        printf("Check capacity failed..\n");
        return e_failure;
    }
    printf("Check capacity is successful..\n");

    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Copy bmp header failed..\n");
        return e_failure;
    }
    printf("Copy bmp header is successful..\n");

    if (encode_magic_string_len(strlen(encInfo->magic_string), encInfo) == e_failure)
    {
        printf("Magic string length encoding failed\n");
        return e_failure;
    }
    if(encode_magic_string(encInfo->magic_string, encInfo) == e_failure)
    {
        printf("Magic string is failed..\n");
        return e_failure;
    }
    printf("Magic string is successful..\n");

    char *res = strrchr(encInfo->secret_fname,'.');
    if(res == NULL)
    {
        printf("Secret file has no extension..\n");
        return -1;
    }
    int extn_len = strlen(res);
    if(encode_secret_file_extn_len(extn_len, encInfo) == e_failure)
    {
        printf("Secret file extension length is failed..\n");
        return e_failure;
    }
    if(encode_secret_file_extn(res, encInfo) == e_failure)
    {
        printf("Secret file extension is failed..\n");
        return e_failure;
    }
    printf("Secret file extension is successful..\n");
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("Secret file size is failed..\n");
        return e_failure;
    }
    printf("Secret file size is successfull..\n");
    if(encode_secret_file_data(encInfo) == e_failure)
    {
        printf("Secret file data is failed..\n");
        return e_failure;
    }
    printf("Secret file data is successfull..\n");
    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("Copy remaining data is failed..\n");
        return e_failure;
    }

    return e_success;

}