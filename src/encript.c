#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bmp.h"
#include "utils.h"
#include "encript.h"

void print_coefficients(unsigned char * coefficients[], int rows, int cols);

int encript(image_t* secret, const char* directory, int k, image_t** shadows, int shadow_amount) {
	DIR* p_dir;
	struct dirent *dir;
	p_dir = opendir(directory);
	int image_count = 0;
  char * full_path;

	if(p_dir) {
    while ((dir = readdir(p_dir)) != NULL) {
   	 	if(strstr(dir->d_name, ".bmp") && image_count < shadow_amount) {
   	 		full_path = calloc(strlen(directory) + strlen(dir->d_name) + 2, 1);
        strcpy(full_path, directory);
        strcat(full_path, "/");
        strcat(full_path, dir->d_name);
        shadows[image_count] = load_bitmap_file(full_path);
   	 		if (shadows[image_count] == NULL) {
          printf("Incorrect image format for image %s\n", full_path);
          exit(EXIT_FAILURE);
        }
        image_count++;
   	 	}
    }
    closedir(p_dir);
  }
  int i;
  hide(shadows, secret, k, image_count);
  return image_count;
}

void hide(image_t** images, image_t* secret, int k, int image_count) {
  if(k == 2) {
    hide_2(images, secret, image_count);
  } else if (k == 3) {
    hide_3(images, secret, image_count);
  }
}

void hide_2(image_t** shadows, image_t* secret, int image_count) {
  srand(time(NULL));
  unsigned char secret_bytes[2];
  unsigned char** shadow_bytes;
  int i=0, j=0;
  int size = secret->info_header.bi_width * secret->info_header.bi_height;

  if(size % 2 != 0) {
    // TODO: Show error
    exit(EXIT_FAILURE);
  }

  shadow_bytes = malloc(image_count * sizeof(char*));
  for(i = 0 ; i < image_count ; i++) {
    shadow_bytes[i] = malloc(2 * sizeof(char));
  }
  for(i = 0; i < size; i += 2) {
    secret_bytes[0] = secret->bitmap[i];
    secret_bytes[1] = secret->bitmap[i+1];

    for(j = 0; j < image_count; j++) {
      char output[1024];
      unsigned char first_byte = shadows[j]->bitmap[i] >> 4;
      unsigned char second_byte = shadows[j]->bitmap[i+1] >> 5;
      while(shadow_is_ld(first_byte, second_byte, shadow_bytes, j)) {
        randomize_byte_shadow(&first_byte, 4);
        randomize_byte_shadow(&second_byte, 3);
      }

      shadow_bytes[j][0] = first_byte;
      shadow_bytes[j][1] = second_byte;

      int secret_number = first_byte * secret_bytes[0] + second_byte * secret_bytes[1];
      secret_number = secret_number % 251;
      shadows[j]->bitmap[i] = 0;
      shadows[j]->bitmap[i] |= (first_byte << 4);
      shadows[j]->bitmap[i] |= (secret_number >> 4);
      shadows[j]->bitmap[i+1] = 0;
      shadows[j]->bitmap[i+1] |= (second_byte << 5);
      shadows[j]->bitmap[i+1] |= (secret_number & 0X0F);

      char parity_check[17];
      strcpy(parity_check, byte_to_binary(shadows[j]->bitmap[i]));
      strncat(parity_check, byte_to_binary(shadows[j]->bitmap[i + 1]), 3);
      strncat(parity_check, byte_to_binary(shadows[j]->bitmap[i + 1] << 4), 4);
      strcat(parity_check, "0");
      
      int output_length = digest_MD5_util(parity_check, output);
      unsigned char parity = md5_xor(output, output_length);
      if (parity) {
        shadows[j]->bitmap[i+1] |= 0X10;
      } else {
        shadows[j]->bitmap[i+1] &= 0XEF;
      }
    }
  }
}

int shadow_is_ld(unsigned char first_byte, unsigned char second_byte, unsigned char** shadow_bytes, int shadows_block_amount) {
  if (first_byte == 0 || second_byte == 0) {
    return 1;
  }

  int i;
  unsigned char first_inv = modular_inverse(first_byte);
  unsigned char sec_after = (second_byte*first_inv) % 251;  
  for(i = 0 ; i < shadows_block_amount ; i++) {
    unsigned char s_0_inv = modular_inverse(shadow_bytes[i][0]);
    unsigned char s_1_after = (shadow_bytes[i][1] * s_0_inv) % 251;

    if (s_1_after == sec_after) {
      return 1;
    }
  }
  return 0;
}

void randomize_byte_shadow(unsigned char* b, int size) {
  int r = (rand() % size);

  int aux_bit = 1 << r;
  *b ^= aux_bit;
}

void hide_3(image_t** shadows, image_t* secret, int image_count) {
  unsigned char secret_bytes[3];
  unsigned char** shadow_bytes;
  int i=0, j=0;
  int size = secret->info_header.bi_width * secret->info_header.bi_height;

  if(size % 3 != 0) {
    // TODO: Show error
    exit(EXIT_FAILURE);
  }

  shadow_bytes = malloc(10 * sizeof(char*));
  for(i = 0 ; i < 10 ; i++) {
    shadow_bytes[i] = malloc(3 * sizeof(char));
  }
  for(i = 0; i < size; i += 3) {
    secret_bytes[0] = secret->bitmap[i];
    secret_bytes[1] = secret->bitmap[i+1];
    secret_bytes[2] = secret->bitmap[i+2];

    for(j = 0; j < image_count; j++) {
      char output[1024];
      unsigned char first_byte = shadows[j]->bitmap[i] >> 3;
      unsigned char second_byte = shadows[j]->bitmap[i+1] >> 3;
      unsigned char third_byte = shadows[j]->bitmap[i+2] >> 3;

      // while(shadow_is_ld_k3(first_byte, second_byte, shadow_bytes, j)) {
      //   randomize_byte_shadow(&first_byte);
      // }

      shadow_bytes[j][0] = first_byte;
      shadow_bytes[j][1] = second_byte;
      shadow_bytes[j][2] = third_byte;

      int secret_number = first_byte * secret_bytes[0] + second_byte * secret_bytes[1] + third_byte * secret_bytes[2];
      secret_number = secret_number % 251;
      shadows[j]->bitmap[i] &= 0xF8; // 1111 1000
      shadows[j]->bitmap[i] |= (secret_number >> 5);
      shadows[j]->bitmap[i+1] &= 0xF8; 
      shadows[j]->bitmap[i+1] |= (secret_number & 0x1C) >> 2; // 0001 1100 >> 2
      shadows[j]->bitmap[i+2] &= 0xF8; 
      shadows[j]->bitmap[i+2] |= (secret_number & 0X03); 

      char parity_check[25];
      strcpy(parity_check, byte_to_binary(shadows[j]->bitmap[i]));
      strcat(parity_check, byte_to_binary(shadows[j]->bitmap[i + 1]));
      strncat(parity_check, byte_to_binary(shadows[j]->bitmap[i + 2]), 5);
      strncat(parity_check, byte_to_binary(shadows[j]->bitmap[i + 2] << 6), 2);
      strcat(parity_check, "0");
      
      int output_length = digest_MD5_util(parity_check, output);
      unsigned char parity = md5_xor(output, output_length);
      if (parity) {
        shadows[j]->bitmap[i+2] |= 0X04;
      } else {
        shadows[j]->bitmap[i+2] &= 0XFB;
      }
    } 
  } 
}
