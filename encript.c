#include <dirent.h>
#include "bmp.h"

image_t encript(image_t secret, char* directory, int k) {
	DIR* p_dir;
	struct dirent *dir;
	p_dir = opendir(directory);
	image_t * images[10];
	int image_count = 0;

	if(p_dir) {
    while ((dir = readdir(p_dir)) != NULL) {
   	 	if(strstr(dir->d_name, '.bmp') && image_count < 10) {
   	 		images[image_count] = load_bitmap_file(dir->d_name);
   	 		image_count++;
   	 	}
    }
    closedir(p_dir);
  }
  
}

image_t * encript(image_t * secret, char * directory, int k) {
	DIR* p_dir;
	struct dirent *dir;
	p_dir = opendir(directory);
	image_t * images[10];
	int image_count = 0;

	if(p_dir) {
    while ((dir = readdir(p_dir)) != NULL) {
   	 	if(strstr(dir->d_name, '.bmp') && image_count < 10) {
   	 		images[image_count] = load_bitmap_file(dir->d_name);
   	 		image_count++;
   	 	}
    }
    closedir(p_dir);
    
    if (image_count > k) {
        image_t * secret_image = (image_t *) malloc(sizeof(images[0]));
        memcpy(&secret_image->file_header, images[0]->file_header, sizeof(BITMAPFILEHEADER));
        memcpy(&secret_image->info_header, images[0]->info_header, sizeof(BITMAPINFOHEADER));
        secret_image->bitmap = (unsigned char *) malloc(secret_image->bi_size_image);
        int i;
        for (i = 0; i < secret->info_header.bi_size_image; i += k) {
            recover_block(secret_image, images, k, i, image_count); 
        }
    } else {
        // TODO: Add coherent error message here
        return NULL;
    }
  } else {
    return NULL;
  } 
}

void
recover_block(image_t * secret_image, image_t ** images, int k, int block_position, int image_count) {
    switch(k) {
        case 2:
            recover_block2(secret_image, images, block_position, image_count);
            break;
        case 3:
            // TODO: Add recovery for k = 3
            break;
    }
}

void
recover_block(image_t * secret_image, image_t ** images, int block_position, int image_count) {
    char coefficients[image_count][3]; 
    int i;
    for (i = 0; i < image_count; i++) {
        coefficients[i][0] = images[i]->bitmap[block_position] >> 4;
        coefficients[i][1] = images[i]->bitmap[block_position + 1] >> 4;
        coefficients[i][2] = ((images[i]->bitmap[block_position] & 15) << 4) | (images[i]->bitmap[block_position] & 15);
    }
    
    
}

