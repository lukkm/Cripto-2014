#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bmp.h"
#include "utils.h"
#include "decript.h"

image_t * recovery(const char * directory, int k) {
	DIR* p_dir;
	struct dirent *dir;
	p_dir = opendir(directory);
	image_t * images[10];
	int image_count = 0;
  char * full_path;

	if(p_dir) {
    while ((dir = readdir(p_dir)) != NULL) {
   	 	if(strstr(dir->d_name, ".bmp") && image_count < 10) {
   	 		full_path = calloc(strlen(directory) + strlen(dir->d_name) + 2, 1);
        strcpy(full_path, directory);
        strcat(full_path, "/");
        strcat(full_path, dir->d_name);
        images[image_count] = load_bitmap_file(full_path);
   	 		image_count++;
        free(full_path);
   	 	}
    }
    closedir(p_dir);
    
    if (image_count >= k) {
        image_t * secret_image = (image_t *) malloc(sizeof(image_t));
        memcpy(&secret_image->file_header, &images[0]->file_header, sizeof(BITMAPFILEHEADER));
        memcpy(&secret_image->info_header, &images[0]->info_header, sizeof(BITMAPINFOHEADER));
        int second_header_size = images[0]->file_header.b_off_bits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
        secret_image->second_header = malloc(second_header_size);
        memcpy(secret_image->second_header, images[0]->second_header, second_header_size);
        secret_image->bitmap = (unsigned char *) malloc(secret_image->info_header.bi_width * secret_image->info_header.bi_height);
        int i;
        for (i = 0; i < secret_image->info_header.bi_width * secret_image->info_header.bi_height; i += k) {
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
            recover_block3(secret_image, images, block_position, image_count);
            break;
    }
}

void
recover_block2(image_t * secret_image, image_t ** images, int block_position, int image_count) {
    unsigned char coefficients[image_count][3]; 
    unsigned char parity_bit[image_count];
    int i, j;
    for (i = 0; i < image_count; i++) {
        coefficients[i][0] = images[i]->bitmap[block_position] >> 4;
        // Shift 5 times to take parity bit out
        coefficients[i][1] = images[i]->bitmap[block_position + 1] >> 5;
        parity_bit[i] = (images[i]->bitmap[block_position + 1] >> 4) & 0x01;
        coefficients[i][2] = ((images[i]->bitmap[block_position] & 0x0F) << 4) | (images[i]->bitmap[block_position + 1] & 0x0F);
    }
    
    // Take the first item of every equation to 1 to solve.
    for (i = 0; i < image_count; i++) {
        unsigned char first_mod_inverse = modular_inverse(coefficients[i][0]);
        for (j = 0; j < 3; j++) {
            coefficients[i][j] = ((int)coefficients[i][j] * first_mod_inverse) % 251;
        }
    }

    // Solve the system using the first two equations
    for (j = 0; j < 3; j++) {
        int result = ((int)coefficients[1][j] - coefficients[0][j]);
        coefficients[1][j] = (result < 0) ? result + 251 : result;
    }

    if (coefficients[1][0] != 0 || coefficients[1][1] == 0) {
        printf("Something went wrong with the equations!\n");
    }

    // Check the parity bit for errors.
    for (i = 0; i < image_count; i++) {
        char * parity_check = calloc((sizeof(unsigned char) * 2) + 1, 1);
        strcpy(parity_check, byte_to_binary(images[i]->bitmap[block_position]));
        strncat(parity_check, byte_to_binary(images[i]->bitmap[block_position + 1]), 3);
        strncat(parity_check, byte_to_binary(images[i]->bitmap[block_position + 1] << 4), 4);
        strcat(parity_check, "0");
        
        char * output = calloc(1024, 1);
        int output_length = digest_MD5_util(parity_check, output);
        unsigned char parity = md5_xor(output, output_length);
        if (parity != parity_bit[i]) {
            //printf("Parity bit incorrect value at block %d, hash was %d, parity_bit %d\n", block_position, parity, parity_bit[i]);
        }    

        free(output);
    }

    unsigned char y = (coefficients[1][2] * modular_inverse(coefficients[1][1])) % 251;
    unsigned char x = (coefficients[0][2] - (coefficients[0][1] * y)) % 251;

    secret_image->bitmap[block_position] = x;
    secret_image->bitmap[block_position + 1] = y;
} 

void
recover_block3(image_t * secret_image, image_t ** images, int block_position, int image_count) {
    unsigned char coefficients[image_count][4];
    unsigned char parity_bit[image_count];
    int i, j;
    for (i = 0; i < image_count; i++) {
        coefficients[i][0] = images[i]->bitmap[block_position] >> 3;
        coefficients[i][1] = images[i]->bitmap[block_position + 1] >> 3;
        // Shift 6 times to take parity bit out
        parity_bit[i] = (images[i]->bitmap[block_position + 2] >> 2) & 0x01;
        coefficients[i][2] = images[i]->bitmap[block_position + 2] >> 3;
        coefficients[i][3] = ((images[i]->bitmap[block_position] & 7) << 5) | ((images[i]->bitmap[block_position + 1] & 7) << 2) | (images[i]->bitmap[block_position + 2] & 3);
    }
        
    // Take the first item of every equation to 1 to solve.
    for (i = 0; i < image_count; i++) {
        unsigned char first_mod_inverse = modular_inverse(coefficients[i][0]);
        for (j = 0; j < 4; j++) {
            coefficients[i][j] = ((int)coefficients[i][j] * first_mod_inverse) % 251;
        }
    }

    // Substract first row from every other
    for (i = 1; i < image_count; i++) {
        for (j = 0; j < 4; j++) {
            int result = ((int)coefficients[i][j] - coefficients[0][j]);
            coefficients[i][j] = (result < 0) ? result + 251 : result;
        }
    }

    unsigned char aux;
    if (coefficients[1][1] == 0) {
        for (j = 0; j < 4; j++) {
            aux = coefficients[1][j];
            coefficients[1][j] = coefficients[2][j];
            coefficients[2][j] = aux;
        }
    } else {
        if (coefficients[2][1] != 0) {
            // Take the second item of every equation below the first one to 1 to solve.
            for (i = 1; i < image_count; i++) {
                unsigned char second_mod_inverse = modular_inverse(coefficients[i][1]);
                for (j = 1; j < 4; j++) {
                    coefficients[i][j] = ((int)coefficients[i][j] * second_mod_inverse) % 251;
                }
            }

            // Substract second row from every other
            // TODO: Modularize this
            for (i = 2; i < image_count; i++) {
                for (j = 0; j < 4; j++) {
                    int result = ((int)coefficients[i][j] - coefficients[1][j]);
                    coefficients[i][j] = (result < 0) ? result + 251 : result;
                }
            }
        }
    }

    if (coefficients[1][0] != 0 || coefficients[2][0] != 0 || coefficients[2][1] != 0 || coefficients[1][1] == 0 || coefficients[2][2] == 0) {
        printf("Something went wrong with the equations!\n");
    }

    // Check the parity bit for errors.
    for (i = 0; i < image_count; i++) {
        char * parity_check = calloc((sizeof(unsigned char) * 3) + 1, 1);
        strcpy(parity_check, byte_to_binary(images[i]->bitmap[block_position]));
        strcat(parity_check, byte_to_binary(images[i]->bitmap[block_position + 1]));
        strncat(parity_check, byte_to_binary(images[i]->bitmap[block_position + 2]), 5);
        strncat(parity_check, byte_to_binary(images[i]->bitmap[block_position + 2] << 6), 2);
        strcat(parity_check, "0");
        
        char * output = calloc(1024, 1);
        int output_length = digest_MD5_util(parity_check, output);
        unsigned char parity = md5_xor(output, output_length);
        if (parity != parity_bit[i]) {
            //printf("Parity bit incorrect value at block %d, hash was %d, parity_bit %d\n", block_position, parity, parity_bit[i]);
        }    

        free(output);
    }

    // Use the first three equations to solve the system.
    unsigned char z = (coefficients[2][3] * modular_inverse(coefficients[2][2])) % 251;
    unsigned char y = (coefficients[1][3] - (coefficients[1][2] * z)) % 251;
    unsigned char x = (coefficients[0][3] - (coefficients[0][2] * z) - (coefficients[0][1] * y)) % 251;

    secret_image->bitmap[block_position] = x;
    secret_image->bitmap[block_position + 1] = y;
    secret_image->bitmap[block_position + 2] = z;
}

void
print_coefficients(unsigned char ** coefficients, int rows, int cols) {
    int i, j;
    printf("Coefficients\n");
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            printf("%d ", coefficients[i][j]);
        }
        printf("\n");
    }
}