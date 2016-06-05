#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

unsigned char* readBMP(char* filename, int *size, int *width, int *height);

int main(){
  char file[]="lena_gray.bmp";
  int size=0, width=0, height=0;
  unsigned char * lena = readBMP(file, &size, &width, &height);

  printf("Image size: w=%d, h=%d, tot=%d pixels\n", width, height, size);
  printf("Image values:\n");
  for(int i=0; i<size; i++)
    printf("%d ", lena[i]);
  printf("\n");
}

unsigned char* readBMP(char* filename, int *size, int *width, int *height)
{
  int i;
  FILE* f = fopen(filename, "rb");
  unsigned char info[54];
  fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

  // extract image height and width from header
  int w = *(int*)&info[18];
  int h = *(int*)&info[22];

  *width=w;
  *height=h;
  // RGB
  //int size = 3 * width * height;
  
  // Grayscale
  int s = w * h;
  *size=s;

  unsigned char* data = new unsigned char[s]; // allocate s bytes per pixel
  fread(data, sizeof(unsigned char), s, f); // read the rest of the data at once
  fclose(f);

  /*
  // Swap BGR to RGB. Not needed since we have grayscale image.
  for(i = 0; i < size; i += 3)
  {
    unsigned char tmp = data[i];
    data[i] = data[i+2];
    data[i+2] = tmp;
  }
  */

  return data;
}

