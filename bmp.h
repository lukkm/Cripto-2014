typedef unsigned char  BYTE; // 1byte
typedef unsigned short int  WORD; // 2bytes
typedef unsigned int DWORD; //4bytes

#pragma pack(push, 1)

typedef struct tag_BITMAPFILEHEADER
{
    WORD bf_type;  //specifies the file type
    DWORD bf_size;  //specifies the size in bytes of the bitmap file
    WORD bf_reserved1;  //reserved; must be 0
    WORD bf_reserved2;  //reserved; must be 0
    DWORD b_off_bits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

typedef struct tag_BITMAPINFOHEADER
{
    DWORD bi_size;  //specifies the number of bytes required by the struct
    DWORD bi_width;  //specifies width in pixels
    DWORD bi_height;  //species height in pixels
    WORD bi_planes; //specifies the number of color planes, must be 1
    WORD bi_bit_count; //specifies the number of bit per pixel
    DWORD bi_compression;//spcifies the type of compression
    DWORD bi_size_image;  //size of image in bytes
    DWORD bi_XPels_per_meter;  //number of pixels per meter in x axis
    DWORD bi_YPels_per_meter;  //number of pixels per meter in y axis
    DWORD bi_clr_used;  //number of colors used by th ebitmap
    DWORD bi_clr_important;  //number of colors that are important
}BITMAPINFOHEADER;

#pragma pack(pop)

unsigned char * load_bitmap_file(const char *filename, BITMAPINFOHEADER *bitmap_info_header);
