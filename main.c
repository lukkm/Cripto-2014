#include <stdio.h>
#include <stdlib.h>
#include <argtable2.h>
#include "bmp.h"
#include "main.h"

int
main(int argc, char **argv)
{
    /* Mandatory parameters */
    struct arg_lit  * distribute = arg_lit0("d", "distribute", "Distribute the secret image");
    struct arg_lit  * recover = arg_lit0("r", "recover", "Recover a secret image");
    struct arg_file * in  = arg_file0("s", "secret", "<input>", "File to distribute/recover");
    struct arg_int * k  = arg_int0("k", NULL, "<int>", "Necessary shadows to reveal the secret");

    /* Optional parameters */
    struct arg_str * dir = arg_str0(NULL, "dir", "<string>", "Directory to use for distributing/recovering a secret");
    struct arg_int * n = arg_int0("n", NULL, "<int>", "Number of shadows to generate when distributing");

    struct arg_end * end = arg_end(10);

    void * argtable[] = {distribute, recover, in, k, dir, n, end};
    int errors;

    // Parse the arguments and validate them
    errors = arg_parse(argc, argv, argtable);

    /* Validate required parameters */
    if ((distribute->count == 0 && recover->count == 0) || in->count == 0 || k->count == 0) {
        printf("Please input all the required parameters (either '-d' or '-r', -s <file>, -k <int>)\n");
        errors++;
    }

    /* Validate flags */
    if (distribute->count > 0 && recover->count > 0) {
        printf("Cannot use distribute and recover in the same run\n");
        errors++;    
    }

    /* Validate k value */
    if (k->ival[0] < 2 || k->ival[0] > 3) {
        printf("k must be in the range [2, 3]\n");
        errors++;
    }

    /* Validate n value if set */
    if (n->count > 0) {
        if (n->ival[0] < 3 || n->ival[0] > 8) {
            printf("n must be in the range [3, 8]\n");
            errors++;    
        }
    }

    // TODO: Validate image is a bmp file.

    if (errors > 0) {
        arg_print_errors(stdout, end, argv[0]);
        printf("Wrong parameter values\n");
        return 1;
    }
    
    BITMAPINFOHEADER bitmapInfoHeader;
    unsigned char *bitmapData;
    bitmapData = LoadBitmapFile(in->filename[0], &bitmapInfoHeader);

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
