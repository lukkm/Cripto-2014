#include <stdlib.h>
#include <stdio.h>
#include "bmp.h"

image_t *
load_bitmap_file(const char *filename) {
    image_t * image = malloc(sizeof(image_t));
    FILE * file_ptr; //our file pointer

    // Open file in read binary mode
    file_ptr = fopen(filename,"rb");
    if (file_ptr == NULL)
        return NULL;

    // Read the bitmap file header
    fread(&image->file_header, sizeof(BITMAPFILEHEADER), 1, file_ptr);

    // Verify that this is a bmp file by check bitmap id
    if (image->file_header.bf_type !=0x4D42) {
        fclose(file_ptr);
        return NULL;
    }

    // Read the bitmap info header
    int second_header_size = image->file_header.b_off_bits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
    fread(&image->info_header, sizeof(BITMAPINFOHEADER), 1, file_ptr);
    image->second_header = malloc(second_header_size);
    fread(image->second_header, second_header_size, 1, file_ptr);

    // Move file point to the begging of bitmap data
    fseek(file_ptr, image->file_header.b_off_bits, SEEK_SET);

    // Allocate enough memory for the bitmap image data
    image->bitmap = (unsigned char*) malloc(image->info_header.bi_width * image->info_header.bi_height);

    // Verify memory allocation
    if (!image->bitmap) {
        free(image->bitmap);
        fclose(file_ptr);
        return NULL;
    }

    // Read in the bitmap image data
    fread(image->bitmap, image->info_header.bi_width * image->info_header.bi_height, 1, file_ptr);

    // Make sure bitmap image data was read
    if (image->bitmap == NULL) {
        fclose(file_ptr);
        return NULL;
    }

    // Close file and return bitmap iamge data
    fclose(file_ptr);
    return image;
}

void
print_matrix(image_t * image) {
    int i, j;
    for (i = 0; i < image->info_header.bi_width; i++) {
        for (j = 0; j < image->info_header.bi_height; j++) {
            printf("%d ", image->bitmap[i * image->info_header.bi_height + j]);
        }
        printf("\n");
    }
}

void
write_bitmap_file(image_t * image, const char * filename) {

    FILE * out_f = fopen(filename, "wb+");
    // Write header
    fwrite(&image->file_header, sizeof(BITMAPFILEHEADER), 1, out_f);
    fwrite(&image->info_header, sizeof(BITMAPINFOHEADER), 1, out_f);
    fwrite(image->second_header, image->file_header.b_off_bits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER), 1, out_f);
    //Write image
    fseek(out_f, image->file_header.b_off_bits, SEEK_SET);
    int n = fwrite(image->bitmap, image->info_header.bi_width * image->info_header.bi_height, 1, out_f);

    fclose(out_f);
}
