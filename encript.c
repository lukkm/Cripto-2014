#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "bmp.h"
#include "utils.h"
#include "encript.h"

image_t * encript(image_t secret, char* directory, int k) {
	DIR* p_dir;
	struct dirent *dir;
	p_dir = opendir(directory);
	image_t * images[10];
	int image_count = 0;

	if(p_dir) {
    while ((dir = readdir(p_dir)) != NULL) {
   	 	if(strstr(dir->d_name, ".bmp") && image_count < 10) {
   	 		images[image_count] = load_bitmap_file(dir->d_name);
   	 		image_count++;
   	 	}
    }
    closedir(p_dir);
  }
 
  return NULL;
}

image_t * recover(char * directory, int k) {
	DIR* p_dir;
	struct dirent *dir;
	p_dir = opendir(directory);
	image_t * images[10];
	int image_count = 0;

	if(p_dir) {
    while ((dir = readdir(p_dir)) != NULL) {
   	 	if(strstr(dir->d_name, ".bmp") && image_count < 10) {
   	 		images[image_count] = load_bitmap_file(dir->d_name);
   	 		image_count++;
   	 	}
    }
    closedir(p_dir);
    
    if (image_count > k) {
        image_t * secret_image = (image_t *) malloc(sizeof(images[0]));
        memcpy(&secret_image->file_header, &images[0]->file_header, sizeof(BITMAPFILEHEADER));
        memcpy(&secret_image->info_header, &images[0]->info_header, sizeof(BITMAPINFOHEADER));
        secret_image->bitmap = (unsigned char *) malloc(secret_image->info_header.bi_size_image);
        int i;
        for (i = 0; i < secret_image->info_header.bi_size_image; i += k) {
            recover_block(secret_image, images, k, i, image_count); 
        }
        return secret_image;
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
recover_block2(image_t * secret_image, image_t ** images, int block_position, int image_count) {
    char coefficients[image_count][3]; 
    int i, j;
    for (i = 0; i < image_count; i++) {
        coefficients[i][0] = images[i]->bitmap[block_position] >> 4;
        coefficients[i][1] = images[i]->bitmap[block_position + 1] >> 4;
        coefficients[i][2] = ((images[i]->bitmap[block_position] & 15) << 4) | (images[i]->bitmap[block_position] & 15);
    }
    
    // Take the first item of every equation to 1 to solve the equations.
    for (i = 0; i < image_count; i++) {
        int first_mod_inverse = modular_inverse(coefficients[i][0]);
        for (j = 0; j < 3; j++) {
            coefficients[i][j] = (coefficients[i][j] * first_mod_inverse) % 251;
        }
    }

    // Solve the system using the first two equations
    for (j = 0; j < 3; j++) {
        coefficients[1][j] -= coefficients[0][j];
        if (coefficients[1][j] < 0) {
            coefficients[1][j] += 251;
        }
    }

    if (coefficients[1][0] != 0) {
        printf("Something went wrong with the equations!\n");
    }

    unsigned char y = (coefficients[1][2] * modular_inverse(coefficients[1][1])) % 251;
    int x_result = coefficients[0][2] - y;
    x_result = (x_result < 0) ? x_result += 251 : x_result;
    unsigned char x = (x_result * modular_inverse(coefficients[0][1])) % 251;

    secret_image->bitmap[block_position] = x;
    secret_image->bitmap[block_position + 1] = y;
} 

