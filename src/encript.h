#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

image_t* encript(image_t* secret, const char* directory, int k);
void hide(image_t** images, image_t* secret, int k);
void hide_2(image_t** images, image_t* secret);
int ld_for_shadow(unsigned char first_byte, unsigned char second_byte, unsigned char** shadow_bytes, int shadows_block_amount);
void randomize_byte_shadow(unsigned char* b);
image_t** hide_3(image_t** images, image_t* secret);
image_t * recovery(const char * directory, int k);
void recover_block(image_t * secret_image, image_t ** images, int k, int block_position, int image_count);
void recover_block2(image_t * secret_image, image_t ** images, int block_position, int image_count);
void recover_block3(image_t * secret_image, image_t ** images, int block_position, int image_count);
