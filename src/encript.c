#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bmp.h"
#include "utils.h"
#include "encript.h"

void print_coefficients(unsigned char ** coefficients, int rows, int cols);

int encript(image_t* secret, const char* directory, int k, image_t** shadows) {
	DIR* p_dir;
	struct dirent *dir;
	p_dir = opendir(directory);
	int image_count = 0;
  char * full_path;

	if(p_dir) {
    while ((dir = readdir(p_dir)) != NULL) {
   	 	if(strstr(dir->d_name, ".bmp") && image_count < 10) {
   	 		full_path = calloc(strlen(directory) + strlen(dir->d_name) + 2, 1);
        strcpy(full_path, directory);
        strcat(full_path, "/");
        strcat(full_path, dir->d_name);
        shadows[image_count] = load_bitmap_file(full_path);
   	 		image_count++;
   	 	}
    }
    closedir(p_dir);
  }
  int i;
/*  for (i = 0; i < image_count; i++) {
      memcpy(&shadows[i]->file_header, &secret->file_header, sizeof(BITMAPFILEHEADER));
      memcpy(&shadows[i]->info_header, &secret->info_header, sizeof(BITMAPINFOHEADER));
      int second_header_size = secret->file_header.b_off_bits - sizeof(BITMAPFILEHEADER) - sizeof(BITMAPINFOHEADER);
      shadows[i]->second_header = malloc(second_header_size);
      memcpy(shadows[i]->second_header, secret->second_header, second_header_size);
      secret_image->bitmap = (unsigned char *) malloc(secret_image->info_header.bi_width * secret_image->info_header.bi_height);
  }*/
  hide(shadows, secret, k, image_count);
  return image_count;
}

void hide(image_t** images, image_t* secret, int k, int image_count) {
  if(k == 2) {
    hide_2(images, secret, image_count);
  } else if (k == 3) {
    // hide_3(images, secret);
  }
}

void hide_2(image_t** shadows, image_t* secret, int image_count) {
  unsigned char secret_bytes[2];
  unsigned char** shadow_bytes;
  int i=0, j=0;
  int size = secret->info_header.bi_width * secret->info_header.bi_height;

  if(size % 2 != 0) {
    printf("lcdtm olboiz");
    return;
  }

  shadow_bytes = malloc(10 * sizeof(char*));
  for(i = 0 ; i < 10 ; i++) {
    shadow_bytes[i] = malloc(2 * sizeof(char));
  }
  i = 0;
  while(i < size) {
    secret_bytes[0] = secret->bitmap[i];
    secret_bytes[1] = secret->bitmap[i+1];
    while(j < image_count) {
      unsigned char first_byte = shadows[j]->bitmap[i] >> 4;
      unsigned char second_byte = shadows[j]->bitmap[i+1] >> 4;
      while(shadow_is_ld(first_byte, second_byte, shadow_bytes, j)) {
        randomize_byte_shadow(&first_byte);
      }
      shadow_bytes[j][0] = first_byte;
      shadow_bytes[j][1] = second_byte;

      int secret_number = first_byte * secret_bytes[0] + second_byte * secret_bytes[1];
      secret_number = secret_number % 251;
      shadows[j]->bitmap[i] &= 0xF0;
      shadows[j]->bitmap[i] |= (secret_number >> 4);
      shadows[j]->bitmap[i+1] &= 0xF0;
      shadows[j]->bitmap[i+1] |= (secret_number & 0X0F);
      j++;
    }
    i += 2;
  }
}

int shadow_is_ld(unsigned char first_byte, unsigned char second_byte, unsigned char** shadow_bytes, int shadows_block_amount) {
  int i;
  for(i = 0 ; i < shadows_block_amount ; i++) {
    if( (first_byte % shadow_bytes[i][0] == 0) || (shadow_bytes[i][0] % first_byte == 0) ) {
      int first_multiplier = MAX(first_byte, shadow_bytes[i][0])/MIN(first_byte, shadow_bytes[i][0]);
      if( (second_byte % shadow_bytes[i][1] == 0) || (shadow_bytes[i][1] % second_byte == 0) ) {
        int second_multiplier = MAX(first_byte, shadow_bytes[i][1])/MIN(first_byte, shadow_bytes[i][1]);
        if(first_multiplier == second_multiplier) {
          return 1;
        }
      }
    }
  }
  return 0;
}

void randomize_byte_shadow(unsigned char* b) {
  
  if (*b != 0) {
    *b -= 1;
  } else {
    *b += 1;
  }
  
}

// image_t** hide_3(image_t** images, image_t* secret) {
//   unsigned char bytes[3];
//   int i=0, j=0;
//   while(i < secret->info_header.bi_size - (info_header.bi_size % 3)) {
//     bytes[0] = secret->bitmap[i];
//     bytes[1] = secret->bitmap[i+1];
//     bytes[2] = secret->bitmap[i+2];
//     while(images[j]) {
//       int first = images[j]->bitmap[i];
//       int second = images[j]->bitmap[i+1];
//       int secret_number = first >> 4 * bytes[0] + second >> 4 * bytes[1];
//       secret_number = secret_number % 251;
//       images[j]->bitmap[i] &= 0xF0;
//       images[j]->bitmap[i] |= secret_number >> 4;
//       images[j]->bitmap[i+1] &= 0xF0;
//       images[j]->bitmap[i+1] |= secret_number & 0X0F;
//       j++;
//     }
//     i+=2;
//   }  
// }

/*
**
** DESENCRIPTION
**
*/

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
    int i, j;
    for (i = 0; i < image_count; i++) {
        coefficients[i][0] = images[i]->bitmap[block_position] >> 4;
        // Shift 5 times to take parity bit out
        coefficients[i][1] = images[i]->bitmap[block_position + 1] >> 5;
        coefficients[i][2] = ((images[i]->bitmap[block_position] & 15) << 4) | (images[i]->bitmap[block_position + 1] & 15);
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

    unsigned char y = (coefficients[1][2] * modular_inverse(coefficients[1][1])) % 251;
    unsigned char x = (coefficients[0][2] - (coefficients[0][1] * y)) % 251;

    secret_image->bitmap[block_position] = x;
    secret_image->bitmap[block_position + 1] = y;
} 

void
recover_block3(image_t * secret_image, image_t ** images, int block_position, int image_count) {
    unsigned char coefficients[image_count][4]; 
    int i, j;
    for (i = 0; i < image_count; i++) {
        coefficients[i][0] = images[i]->bitmap[block_position] >> 3;
        coefficients[i][1] = images[i]->bitmap[block_position + 1] >> 3;
        // Shift 6 times to take parity bit out
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
        printf("%d %d %d\n%d %d %d\n", coefficients[1][0], coefficients[1][1], coefficients[1][2], coefficients[2][0], coefficients[2][1], coefficients[2][2]);
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