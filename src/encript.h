#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int encript(image_t* secret, const char* directory, int k, image_t** image_count);
void hide(image_t** images, image_t* secret, int k, int image_count);
void hide_2(image_t** images, image_t* secret, int image_count);
int shadow_is_ld(unsigned char first_byte, unsigned char second_byte, unsigned char** shadow_bytes, int shadows_block_amount);
void randomize_byte_shadow(unsigned char* b);
void hide_3(image_t** shadows, image_t* secret, int image_count);