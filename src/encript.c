#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <time.h>
#include "bmp.h"
#include "utils.h"
#include "encript.h"

void print_coefficients(unsigned char * coefficients[], int rows, int cols);

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
  for(i = 0; i < size; i += 2) {
    secret_bytes[0] = secret->bitmap[i];
    secret_bytes[1] = secret->bitmap[i+1];
    for(j = 0; j < image_count; j++) {
      unsigned char first_byte = shadows[j]->bitmap[i] >> 4;
      unsigned char second_byte = shadows[j]->bitmap[i+1] >> 5;
      //while(shadow_is_ld(first_byte, second_byte, shadow_bytes, j)) {
      //  randomize_byte_shadow(&first_byte);
      //}
      shadow_bytes[j][0] = first_byte;
      shadow_bytes[j][1] = second_byte;

      int secret_number = first_byte * secret_bytes[0] + second_byte * secret_bytes[1];
      secret_number = secret_number % 251;
      shadows[j]->bitmap[i] &= 0xF0;
      shadows[j]->bitmap[i] |= (secret_number >> 4);
      shadows[j]->bitmap[i+1] &= 0xF0;
      shadows[j]->bitmap[i+1] |= (secret_number & 0X0F);

      EVP_MD_CTX mdctx;
      const EVP_MD *md;
      unsigned char output[EVP_MAX_MD_SIZE];
      int output_len;
      
      char * parity_check = calloc((sizeof(unsigned char) * 2) + 1, 1);
      strcpy(parity_check, byte_to_binary(shadows[j]->bitmap[i]));
      strncat(parity_check, byte_to_binary(shadows[j]->bitmap[i + 1]), 3);
      strncat(parity_check, byte_to_binary(shadows[j]->bitmap[i + 1] << 4), 4);
      strcat(parity_check, "0");
      
      OpenSSL_add_all_digests();
      md = EVP_get_digestbyname("MD5");
      EVP_MD_CTX_init(&mdctx);
      EVP_DigestInit_ex(&mdctx, md, NULL);
      EVP_DigestUpdate(&mdctx, parity_check, strlen(parity_check));
      EVP_DigestFinal_ex(&mdctx, output, &output_len);
      EVP_MD_CTX_cleanup(&mdctx);

      unsigned char parity = md5_xor(output);
      if (parity) {
        shadows[j]->bitmap[i+1] |= 0X10;
      } else {
        shadows[j]->bitmap[i+1] &= 0XEF;
      }
    }
  }
}

int shadow_is_ld(unsigned char first_byte, unsigned char second_byte, unsigned char** shadow_bytes, int shadows_block_amount) {
  int i;
  unsigned char first_inv = modular_inverse(first_byte);
  unsigned char sec_after = (second_byte*first_inv) % 251;
  for(i = 0 ; i < shadows_block_amount ; i++) {
    unsigned char s_0_inv = modular_inverse(shadow_bytes[i][0]);
    unsigned char s_1_after = ((shadow_bytes[i][1]) * s_0_inv) % 251;

    if (s_1_after == sec_after) {
      printf("es ld\n");
      return 1;
    }
  }
  return 0;
}

// ya viene el byte shifteado 4 >>
void randomize_byte_shadow(unsigned char* b) {
  srand(time(NULL));
  int r = (rand() % 3) + 1;

  int aux_bit = 1 << r;
  *b ^= aux_bit;
}

void hide_3(image_t** shadows, image_t* secret, int image_count) {
  unsigned char secret_bytes[3];
  unsigned char** shadow_bytes;
  int i=0, j=0;
  int size = secret->info_header.bi_width * secret->info_header.bi_height;

  if(size % 3 != 0) {
    printf("lcdtm olboiz");
    return;
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
      // falta agregarle el bit de paridad!
      shadows[j]->bitmap[i+2] |= (secret_number & 0X03); 
    } 
  } 
}
