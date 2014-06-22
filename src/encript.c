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
  for(i = 0; i < size; i += 2) {
    secret_bytes[0] = secret->bitmap[i];
    secret_bytes[1] = secret->bitmap[i+1];
    for(j = 0; j < image_count; j++) {
      unsigned char first_byte = shadows[j]->bitmap[i] >> 4;
      unsigned char second_byte = shadows[j]->bitmap[i+1] >> 5;
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
    }
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

