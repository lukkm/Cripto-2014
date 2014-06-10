#include <dirent.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "bmp.h"
#include "encript.h"

image_t* encript(image_t* secret, const char* directory, int k) {
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
  hide(images, secret, k);
}

void hide(image_t** images, image_t* secret, int k) {
  if(k == 2) {
    hide_2(images, secret);
  } else if (k == 3) {
    hide_3(images, secret);
  }
}

void hide_2(image_t** images, image_t* secret) {
  unsigned char secret_bytes[2];
  unsigned char** shadow_bytes;
  int i=0, j=0;

  if(secret->info_header.bi_size % 2!=0) {
    printf("lcdtm olboiz");
    return;
  }

  shadow_bytes = malloc(10 * sizeof(char*));
  for(i = 0 ; i < 10 ; i++) {
    shadow_bytes[i] = malloc(2 * sizeof(char));
  }

  while(i < (secret->info_header.bi_width * secret->info_header.bi_width)) {
    secret_bytes[0] = secret->bitmap[i];
    secret_bytes[1] = secret->bitmap[i+1];
    while(images[j]) {
      unsigned char first_byte = images[j]->bitmap[i] >> 4;
      unsigned char second_byte = images[j]->bitmap[i+1] >> 4;
      while(!ld_for_shadow(first_byte, second_byte, shadow_bytes, j)) {
        randomize_byte_shadow(&first_byte);
      }
      shadow_bytes[j][0] = images[j]->bitmap[i] >> 4;
      shadow_bytes[j][1] = images[j]->bitmap[i+1] >> 4;

      int secret_number = shadow_bytes[j][0] * secret_bytes[0] + shadow_bytes[j][1] * secret_bytes[1];
      secret_number = secret_number % 251;
      images[j]->bitmap[i] &= 0xF0;
      images[j]->bitmap[i] |= secret_number >> 4;
      images[j]->bitmap[i+1] &= 0xF0;
      images[j]->bitmap[i+1] |= secret_number & 0X0F;
      j++;
    }
    i+=2;
  }
}

int ld_for_shadow(unsigned char first_byte, unsigned char second_byte, unsigned char** shadow_bytes, int shadows_block_amount) {
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
    } else if ( (first_byte % shadow_bytes[i][1] == 0) || (shadow_bytes[i][1] % first_byte == 0) ) {
      int first_multiplier = MAX(first_byte, shadow_bytes[i][1])/MIN(first_byte, shadow_bytes[i][1]);
      if( (second_byte % shadow_bytes[i][0] == 0) || (shadow_bytes[i][0] % second_byte == 0) ) {
        int second_multiplier = MAX(first_byte, shadow_bytes[i][0])/MIN(first_byte, shadow_bytes[i][0]);
        if(first_multiplier == second_multiplier) {
          return 1;
        }
      }
    }
  }
  return 0;
}

void randomize_byte_shadow(unsigned char* b) {
  srand(time(NULL));
  int r = (rand() % 8) + 1;

  switch (r) {
    case 8:
      if(*b >> 7 % 2 == 1) {
        *b = *b & 0x7F;
      } else {
        *b = *b | 0x80;
      }
      break;
    case 7:
      if(*b >> 6 % 2 == 1) {
        *b = *b & 0xBF;
      } else {
        *b = *b | 0x40;
      }
      break;
    case 6:
      if(*b >> 5 % 2 == 1) {
        *b = *b & 0xDF;
      } else {
        *b = *b | 0x20;
      }
      break;
    case 5:
      if(*b >> 4 % 2 == 1) {
        *b = *b & 0xEF;
      } else {
        *b = *b | 0x10;
      }
      break;
    case 4:
      if(*b >> 3 % 2 == 1) {
        *b = *b & 0xF7;
      } else {
        *b = *b | 0x08;
      }
      break;
    case 3:
      if(*b >> 2 % 2 == 1) {
        *b = *b & 0xFB;
      } else {
        *b = *b | 0x04;
      }
      break;
    case 2:
      if(*b >> 1 % 2 == 1) {
        *b = *b & 0xFD;
      } else {
        *b = *b | 0x02;
      }
      break;
    case 1:
      if(*b % 2 == 1) {
        *b = *b & 0xFE;
      } else {
        *b = *b | 0x01;
      }
      break;
  }
}

image_t** hide_3(image_t** images, image_t* secret) {
  // unsigned char bytes[3];
  // int i=0, j=0;
  // while(i < secret->info_header.bi_size - (info_header.bi_size % 3)) {
  //   bytes[0] = secret->bitmap[i];
  //   bytes[1] = secret->bitmap[i+1];
  //   bytes[2] = secret->bitmap[i+2];
  //   while(images[j]) {
  //     int first = images[j]->bitmap[i];
  //     int second = images[j]->bitmap[i+1];
  //     int secret_number = first >> 4 * bytes[0] + second >> 4 * bytes[1];
  //     secret_number = secret_number % 251;
  //     images[j]->bitmap[i] &= 0xF0;
  //     images[j]->bitmap[i] |= secret_number >> 4;
  //     images[j]->bitmap[i+1] &= 0xF0;
  //     images[j]->bitmap[i+1] |= secret_number & 0X0F;
  //     j++;
  //   }
  //   i+=2;
  // }  
}