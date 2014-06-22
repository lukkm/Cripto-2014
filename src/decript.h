image_t * recovery(const char * directory, int k);
void recover_block(image_t * secret_image, image_t ** images, int k, int block_position, int image_count);
void recover_block2(image_t * secret_image, image_t ** images, int block_position, int image_count);
void recover_block3(image_t * secret_image, image_t ** images, int block_position, int image_count);