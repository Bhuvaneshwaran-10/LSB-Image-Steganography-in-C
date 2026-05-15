
/*
                                --- Image Steganography (LSB Technique) ---

     Name : Bhuvaneshwaran P
       ID : 25036B_092
Start date: 10/02/2026
 End date : 25/02/2026

DESCRIPTION:
             This project implements Image Steganography in C using the Least Significant Bit (LSB) technique
 to securely hide secret data inside a BMP image. The system embeds a magic string, file extension, 
 file size, and the actual secret content into the image without visibly changing its appearance. 
 During decoding, the hidden information is extracted and reconstructed into the original file.
 The project focuses on binary file processing, bit-level data manipulation, and structured program design.
 It demonstrates practical implementation of secure data hiding concepts and basic cybersecurity techniques.

 */


#include <stdio.h>
#include<string.h>
#include"decode.h"
#include "encode.h"
#include "types.h"

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

int main(int argc, char *argv[])
{
    //res = check_operationType(argv);

    //res -> e_encode -> read_and_validate_encodde_arg(); -> do_encoding();

    //res -> e_decode -> read_and_validate_decode_arg();  -> do_decoding();

    //res -> e_usupported -> error -> terminate

    if(argc < 2)
    {
        printf("Usage:\n");
        printf("ENCODE ===> ./a.out -e <.bmp file> <.txt file> [output file]\n");
        printf("DECODE ===> ./a.out -d <.bmp file> [output file]\n");
        return e_failure;
    }
    if(check_operation_type(argv) == e_encode)
    {
        if (argc < 4)
        {
            printf("Usage:\n");
            printf("For Encoding : ./a.out -e <.bmp file> <.txt file> [output file]\n");
            return e_failure;
        }

        printf("You have choosed encoding\n");
        EncodeInfo encInfo;

        if(read_and_validate_encode_args(argv, &encInfo) == e_failure)
        {
            printf("Argument validation failed.\n");
            return e_failure;
        }
        printf("Argument validation Done\n");

        printf("Enter Magic string : ");
        scanf(" %[^\n]", encInfo.magic_string);   
        encInfo.magic_size = strlen(encInfo.magic_string);

        if (do_encoding(&encInfo) == e_failure)
        {
            printf("Encoding not completed!\n");
            return e_failure;
        }
        printf("---------------------------------------------\n");
        printf("Encoding completed successfully!\n");
        printf("---------------------------------------------\n");
        return e_success;
    }
    else if(check_operation_type(argv) == e_decode)
    {
        if (argc < 3)   
        {
            printf("Usage:\n");
            printf("For Decoding : ./a.out -d <.bmp file> [output file]\n");
            return e_failure;
        }

        printf("You have choosed decoding\n");
        DecodeInfo decInfo;

        if(read_and_validate_decode_args(argv, &decInfo) == e_failure)
        {
            printf("Argument validation failed.\n");
            return e_failure;
        }
        printf("Argument validation Done\n");

        printf("Enter Magic string : ");
        scanf(" %[^\n]", decInfo.magic_string);  
        decInfo.magic_size = strlen(decInfo.magic_string);

        if (do_decoding(&decInfo) == e_failure)
        {
            printf("Decoding not completed!\n");
            return e_failure;
        }
        printf("---------------------------------------------\n");
        printf("Decoding Completed successfully\n");
        printf("---------------------------------------------\n");
        return e_success;
    }
    else
    {
        printf("Enter correct argument\n");
        return e_failure;
    }
}
