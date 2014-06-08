typedef unsigned char pixel_t;

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t header_size;
    char * header;
    pixel_t ** data;
} image_t;

void BMP_write(image_t * image);
image_t * BMP_read(char * filename, uint32_t width, uint32_t height);
void BMP_print_matrix(image_t * bmpimage);
