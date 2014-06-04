typedef unsigned char pixel_t;

#ifndef uint32_t
typedef unsigned int uint32_t;
#endif

typedef struct {
    uint32_t width;
    uint32_t height;
    pixel_t ** data;
} image_t; 
