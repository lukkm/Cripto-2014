#include <dirent.h>

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