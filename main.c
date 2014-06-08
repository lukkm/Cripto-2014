#include <stdio.h>
#include <stdlib.h>
#include <argtable2.h>
#include "main.h"

int
main(int argc, char **argv)
{
    int i, j;

    if (argc < 4) {
        printf("Usage: %s filename width height.\n", argv[0]);
        return 1;
    }

    image_t * bmpimage = BMP_read(argv[1], atoi(argv[2]), atoi(argv[3]));
    BMP_write(bmpimage);

    return 0;
}

image_t * 
BMP_read(char * filename, uint32_t width, uint32_t height) {
    int i, j;

    FILE * f = fopen(filename, "rw");
    image_t * bmpimage = malloc(sizeof(image_t));
    bmpimage->width = width;
    bmpimage->height = height;
    bmpimage->data = malloc(bmpimage->width * sizeof(pixel_t *));
    
    // Get the total size in bytes of the image (including header)
    fseek(f, 0L, SEEK_END);
    int total_size = ftell(f);
    
    printf("%d\n", total_size);
    printf("%d\n", bmpimage->width * bmpimage->height);
    bmpimage->header_size = total_size - (bmpimage->width * bmpimage->height);

    for (i = 0; i < bmpimage->width; i++) {
        bmpimage->data[i] = malloc(bmpimage->height);
    }

    bmpimage->header = malloc(bmpimage->header_size * sizeof(char));
    fread(&bmpimage->header, bmpimage->header_size * sizeof(char), 1, f);

    // Seek to the beggining of the bitmap matrix
    fseek(f, bmpimage->header_size, SEEK_SET);

    for (i = 0; i < bmpimage->width; i++) {
        for (j = 0; j < bmpimage->height; j++) {
            size_t read = fread(&bmpimage->data[i][j], sizeof(pixel_t), 1, f);
        }
    }

    fclose(f);

    return bmpimage;
}

void 
BMP_write(image_t * image) {
    int i, j;

    FILE *file;
    file = fopen("generated_bitmap.bmp", "w+");
  

    fwrite(image->header, image->header_size, 1, file);

    for (i = 0; i < image->width; i++) {
        for (j = 0; j < image->height; j++) {
            fputc(image->data[i][j], file);
        }
    }
    fclose(file);
}

void
BMP_print_matrix(image_t * bmpimage) {
    int i, j;
    for (i = 0; i < bmpimage->width; i++) {
        for (j = 0; j < bmpimage->height; j++) {
            printf("%d ", bmpimage->data[i][j]);
        }
        printf("\n");
    }
}
