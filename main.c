#include <stdio.h>
#include <stdlib.h>
#include "main.h"

int
main(int argc, char **argv)
{
    int i, j;

    if (argc < 4) {
        printf("Usage: %s filename width height.\n", argv[0]);
        return 1;
    }

    FILE * f = fopen(argv[1], "rw");
    image_t * bmpimage = malloc(sizeof(image_t));
    bmpimage->width = atoi(argv[2]); 
    bmpimage->height = atoi(argv[3]);
    bmpimage->data = malloc(bmpimage->width * sizeof(pixel_t *));

    // Get the total size in bytes of the image (including header)
    fseek(f, 0L, SEEK_END);
    int total_size = ftell(f);

    printf("%d\n", bmpimage->width);
    for (i = 0; i < bmpimage->width; i++) {
        bmpimage->data[i] = malloc(bmpimage->height);
    }

    // Seek to the beggining of the bitmap matrix
    long seek_to = total_size - (bmpimage->width * bmpimage->height);
    fseek(f, seek_to, SEEK_SET);


    for (i = 0; i < bmpimage->width; i++) {
        for (j = 0; j < bmpimage->height; j++) {
            size_t read = fread(&bmpimage->data[i][j], sizeof(pixel_t), 1, f);
        }
    }
    
    for (i = 0; i < bmpimage->width; i++) {
        for (j = 0; j < bmpimage->height; j++) {
            printf("%d ", bmpimage->data[i][j]);
        }
        printf("\n");
    }

    return 0;
}
