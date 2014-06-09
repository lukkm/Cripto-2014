#include <stdlib.h>
#include <stdio.h>
#include "bmp.h"

unsigned char *
load_bitmap_file(const char *filename, BITMAPINFOHEADER *bitmap_info_header) {
    FILE * file_ptr; //our file pointer
    BITMAPFILEHEADER bitmap_file_header; //our bitmap file header
    unsigned char *bitmap_image;  //store image data
    int image_idx=0;  //image index counter
    unsigned char temp_RGB;  //our swap variable

    //open filename in read binary mode
    file_ptr = fopen(filename,"rb");
    if (file_ptr == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmap_file_header, sizeof(BITMAPFILEHEADER),1,file_ptr);

    //verify that this is a bmp file by check bitmap id
    if (bitmap_file_header.bf_type !=0x4D42)
    {
        fclose(file_ptr);
        return NULL;
    }

    //read the bitmap info header
    fread(bitmap_info_header, sizeof(BITMAPINFOHEADER),1,file_ptr); // small edit. forgot to add the closing bracket at sizeof

    //move file point to the begging of bitmap data
    fseek(file_ptr, bitmap_file_header.b_off_bits, SEEK_SET);

    //allocate enough memory for the bitmap image data
    bitmap_image = (unsigned char*)malloc(bitmap_info_header->bi_size_image);

    //verify memory allocation
    if (!bitmap_image)
    {
        free(bitmap_image);
        fclose(file_ptr);
        return NULL;
    }

    //read in the bitmap image data
    fread(bitmap_image,bitmap_info_header->bi_size_image, 1, file_ptr);

    //make sure bitmap image data was read
    if (bitmap_image == NULL)
    {
        fclose(file_ptr);
        return NULL;
    }

    //close file and return bitmap iamge data
    fclose(file_ptr);
    return bitmap_image;
}
