#include <cstdio>
#include "bitmap_image.hpp"

using namespace std;

int main()
{
   bitmap_image image("input.bmp");

   if (!image)
   {
      printf("Error - Failed to open: input.bmp\n");
      return 1;
   }

   unsigned char red;
   unsigned char green;
   unsigned char blue;

   unsigned int total_number_of_pixels = 0;

   const unsigned int height = image.height();
   cout << "height: " << height << endl;
   const unsigned int width  = image.width();
   cout << "width " << width << endl;

   // Count pixels with >= 111 value for red
   // Replace lower x and y values with black stripes
   for (std::size_t y = 0; y < height; ++y)
   {
      for (std::size_t x = 0; x < width; ++x)
      {
         image.get_pixel(x,y,red,green,blue);
         if (red >= 111)
            total_number_of_pixels++;
         if (y % 2 == 0 && y < height/2 && x < width/2) {
        	 image.set_pixel(x, y, 0, 0, 0);
         }
      }
   }

   image.save_image("output.bmp");

   printf("Number of pixels with red >= 111: %d\n",total_number_of_pixels);
   return 0;
}
