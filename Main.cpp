#include "bitmap_image.hpp"
#include "BlockUtility.h"
#include "StegoCommon.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>

using namespace std;

/* Input perimeters*/
bool analysisFlag = false;
bool encodeFlag = false;
bool decodeFlag = false;
bool redFlag = false;
bool greenFlag = false;
bool blueFlag = false;
bool allFlag = false;
float complexityTH = -1.0;
string coverFileName = "";
string messageFileName = "";
string stegoFileName = "";
string outputFileName = "";
BlockUtility* utility;

void verifyArguments(int argc, char* argv[]);
void encode();

int main(int argc, char* argv[])
{
    verifyArguments(argc, argv);
    utility = new BlockUtility();
    utility->setComplexityThreshold(complexityTH);
    encode();
}

void verifyArguments(int argc, char* argv[])
{
    string usage =  "Stegy -a -c alpha  < -red | -green | -blue | -all >"
    		"-cover CoverImage\nStegy -e -c alpha  < -red | -green | -blue |"
    		"-all > -cover CoverImage -messgae Message -o OutputFileName\nStegy"
    		"-d -c alpha  < -red | -green | -blue | -all > -stego StegoImage"
    		"-o OutputFileName\n";

    if(argc < 5)
    {
        cout << "Usage : " << usage << endl;
        exit(1);
    }
    
    for(int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i],"-a") == 0)
        {
            analysisFlag = true;
        }
        else if(strcmp(argv[i],"-e") == 0)
        {
            encodeFlag = true;
        }
        else if(strcmp(argv[i],"-d") == 0)
        {
            decodeFlag = true;
        }
        else if(strcmp(argv[i],"-c") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage : " << usage << endl;
                exit(1);
            }
            try
            {
                complexityTH = stof(argv[i+1]);
            }
            catch(...)
            {
                cout << "Invalid Complexity : " << argv[i+1] << endl;
                exit(1);
            }
            i++;
        }
        if(strcmp(argv[i],"-red") == 0)
        {
            redFlag = true;
        }
        else if(strcmp(argv[i],"-green") == 0)
        {
            greenFlag = true;
        }
        else if(strcmp(argv[i],"-blue") == 0)
        {
            blueFlag = true;
        }
        else if(strcmp(argv[i],"-all") == 0)
        {
            allFlag = true;
        }
        else if(strcmp(argv[i],"-cover") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage : " << usage << endl;
                exit(1);
            }
            coverFileName = argv[i+1];
            i++;
        }
        else if(strcmp(argv[i],"-stego") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage : " << usage << endl;
                exit(1);
            }
            stegoFileName = argv[i+1];
            i++;
        }
        else if(strcmp(argv[i],"-message") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage : " << usage << endl;
                exit(1);
            }
            messageFileName = argv[i+1];
            i++;
        }
        else if(strcmp(argv[i],"-o") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage : " << usage << endl;
                exit(1);
            }
            outputFileName = argv[i+1];
            i++;
        }
    }
    
    if(!(analysisFlag || encodeFlag || decodeFlag))
    {
        cout << "Usage : " << usage << endl;
        exit(1);
    }
    if(!(redFlag || blueFlag || greenFlag || allFlag))
    {
        cout << "Usage : " << usage << endl;
        exit(1);
    }
    if(complexityTH > 0.5 || complexityTH < 0)
    {
        cout << "Complexity Threshold must be between 0 and 0.5. Found value : " << complexityTH << endl;
        exit(1);
    }
    if(analysisFlag && coverFileName.empty())
    {
        cout << "Need cover file name" << endl;
        exit(1);
    }
    if(encodeFlag && ( coverFileName.empty() || messageFileName.empty() || outputFileName.empty() ))
    {
        cout << "Need proper file names\n"<< usage << endl;
        exit(1);
    }
    if(decodeFlag && ( stegoFileName.empty() || outputFileName.empty() ))
    {
        cout << "Need proper file names\n"<< usage << endl;
        exit(1);
    }
}

void encode() {
	// Open bitmap file
   bitmap_image cover(coverFileName);

   if (!cover)
   {
	  cout << "Error: Unable to open cover image file: "
			  << coverFileName << endl;
	  exit(1);
   }
   // traverse lowest bitplane
   unsigned char red;
   unsigned char green;
   unsigned char blue;
   unsigned char pixelBlock[kBlockSize][kBlockSize];
   int bitPlane = 0;
   int count = 0;
   for (int x = 0; x <= cover.width() - kBlockSize; x += kBlockSize) {
	   for (int y = 0; y <= cover.height() - kBlockSize; y += kBlockSize) {
		   for (size_t i = x; i < x + kBlockSize; i++) {
			   for (size_t j = y; j < y + kBlockSize; j++) {
				   cover.get_pixel(i, j, red, green, blue);
				   pixelBlock[i][j] = red;
//				   cout << "i: " << i << " J: " << j << endl;
//				   cout << "color values: red: " << (int)  red << " green: " << (int) green
//						   << "blue: " << (int )blue << endl;
			   }
		   }
		   unsigned char block[kBlockSize];
		   utility->extractBitPlane(pixelBlock, block, bitPlane);
		   float alpha = utility->getComplexity(block);
		   cout << "complexity: " << alpha << endl;
		   if (alpha > complexityTH) {
			   cout << "found complex" << endl;
			   cout << "alpha: " << alpha << endl;
			   utility->printBitPlane(block);
		   }
	   }
	   count++;
	   cout << "loop: " << count << endl;
   }

}
