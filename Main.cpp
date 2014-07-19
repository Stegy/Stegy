#include "bitmap_image.hpp"
#include "BlockUtility.h"
#include "CGCTranslator.h"
#include "MessageReader.h"
#include "StegoCommon.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

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
MessageReader* reader;
CGCTranslator* cgc;
int blockIndex = 0;
int messageSize = 0;

int kRed = 0;
int kGreen = 1;
int kBlue = 2;

struct MapBlockCoords {
	pair<int, int> pixelCorner;
	int bitPlane;
	int color;
};

vector<MapBlockCoords *> mapBlockCoords;

void verifyArguments(int argc, char* argv[]);
void encode();
void decode();
bool traverseBitPlaneEmbed(int bitPlane, int color, bitmap_image* image);
void imageToCGC(bitmap_image* image);
void imageToPBC(bitmap_image* image);

int main(int argc, char* argv[])
{
    verifyArguments(argc, argv);
    utility = new BlockUtility();
    utility->setComplexityThreshold(complexityTH);
    cgc = new CGCTranslator();
    if (encodeFlag) {
    	encode();
    } else if (decodeFlag) {
    	decode();
    }
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
   // Open message file
   reader = new MessageReader(messageFileName, utility);
   // TODO handle errors
//   int size = reader->getSize();
//   cout << "found size" << size << endl;

   // Translate image from BPC to CGC
   imageToCGC(&cover);

   // Traverse bit planes from lowest to highest
   bool done = false;
   for (int i = 0; i < 8 && !done; i++) {
	   if (redFlag || allFlag) {
		   done = traverseBitPlaneEmbed(i, kRed, &cover);
	   }
	   if (greenFlag || allFlag) {
		   done = traverseBitPlaneEmbed(i, kGreen, &cover);
	   }
	   if (blueFlag || allFlag) {
		   done = traverseBitPlaneEmbed(i, kBlue, &cover);
	   }
   }

   imageToPBC(&cover);

   // Write bitmap with embedded data to file
   cover.save_image(outputFileName);

   // traverse lowest bitplane - goes through 1 bitplane of 1 color
//   unsigned char red;
//   unsigned char green;
//   unsigned char blue;
//   int color = kRed;
//   unsigned char pixelBlock[kBlockSize][kBlockSize];
//   unsigned char block[kBlockSize];
//   int bitPlane = 0;
//   for (int x = 0; x <= cover.width() - kBlockSize; x += kBlockSize) {
//	   for (int y = 0; y < cover.height() - kBlockSize; y += kBlockSize) {
//		   for (size_t i = 0; i < kBlockSize; i++) {
//			   for (size_t j = 0; j < kBlockSize; j++) {
//				   cover.get_pixel(x + i, y + j, red, green, blue);
//				   pixelBlock[i][j] = red;
//			   }
//		   }
//		   utility->extractBitPlane(pixelBlock, block, bitPlane);
//		   float alpha = utility->getComplexity(block);
//		   if (utility->isComplex(block)) {
//			   // Complex block found
//			   if (blockIndex > 0 && blockIndex <= reader->getNumMapBlocks()) {
//				   MapBlockCoords mbc;
//				   mbc.pixelCorner.first = x;
//				   mbc.pixelCorner.second = y;
//				   mbc.bitPlane = bitPlane;
//				   mbc.color = color;
//				   mapBlockCoords.push_back(&mbc);
//			   } else {
//				   if (blockIndex == 0) {
//					   reader->getHeader(block);
//				   }  else {
//					   // Normal message block
//					   reader->getNext(kBlockSize, block);
//					   if (utility->getComplexity(block) < complexityTH) {
//						   utility->conjugate(block);
//						   reader->setMapBit(blockIndex, true);
//					   }
//				   }
//				   utility->embedBitPlane(pixelBlock, block, bitPlane);
//				   for (size_t i = 0; i < kBlockSize; i++) {
//					   for (size_t j = 0; j < kBlockSize; j++) {
//						   cover.set_pixel(x + i, y + j, pixelBlock[i][j],
//								   green, blue);
//					   }
//				   }
//			   }
//			   blockIndex++;
//		   }
//	   }
//   }
}

// Returns whether message ended
bool traverseBitPlaneEmbed(int bitPlane, int color, bitmap_image* image) {
   unsigned char red; // Red value of pixel
   unsigned char green; // Green value of pixel
   unsigned char blue; // Blue value of pixel
   // 8x8 block of single color value
   unsigned char pixelBlock[kBlockSize][kBlockSize];
   unsigned char block[kBlockSize]; // 8x8 block from a bit plane
   // Traverse image pixels
   for (int x = 0; x <= image->width() - kBlockSize; x += kBlockSize) {
	   for (int y = 0; y < image->height() - kBlockSize; y += kBlockSize) {
		   // Get color values for 8x8 pixel block
		   for (size_t i = 0; i < kBlockSize; i++) {
			   for (size_t j = 0; j < kBlockSize; j++) {
				   image->get_pixel(x + i, y + j, red, green, blue);
				   if (color == kRed) {
//					   pixelBlock[i][j] = cgc->binaryToCgc(red);
					   pixelBlock[i][j] = red;
				   } else if (color == kGreen) {
					   pixelBlock[i][j] = green;
				   } else {
					   pixelBlock[i][j] = blue;
				   }
			   }
		   }
		   // Get desired bit plane
		   utility->extractBitPlane(pixelBlock, block, bitPlane);
		   if (utility->isComplex(block)) {
			   if (blockIndex > 0 && blockIndex <= reader->getNumMapBlocks()) {
				   // Reserve space for map blocks
				   // Applies to complex blocks 1 -> number of map blocks
				   MapBlockCoords mbc;
				   mbc.pixelCorner.first = x;
				   mbc.pixelCorner.second = y;
				   mbc.bitPlane = bitPlane;
				   mbc.color = color;
				   mapBlockCoords.push_back(&mbc);
			   } else {
				   if (blockIndex == 0) {
					   // First block to embed in - will embed the size block
					   reader->getSizeBlock(block);
				   }  else {
					   // Will embed a normal message block
					   int bytesRead = reader->getNext(kBlockSize, block);
					   if (bytesRead == 0) {
						   cout << "Done with message, size: " << messageSize << endl;
						   cout << "bitplane: " << bitPlane << endl;
						   return true;
					   }
					   messageSize += bytesRead;
					   // Conjugate block if necessary, setting map value
					   if (!utility->isComplex(block)) {
						   utility->conjugate(block);
						   reader->setMapBit(blockIndex, true);
					   }
				   }
				   // Modify the current block of color values to contain the
				   // secret block
				   utility->embedBitPlane(pixelBlock, block, bitPlane);
				   // Set the updated pixel data in the bitmap image
				   for (size_t i = 0; i < kBlockSize; i++) {
					   for (size_t j = 0; j < kBlockSize; j++) {
						   image->set_pixel(x + i, y + j,
//								   cgc->cgcToBinary(pixelBlock[i][j]),
								   pixelBlock[i][j],
								   green, blue);
					   }
				   }
			   }
			   blockIndex++;
		   }
	   }
   }
   cout << "finished bit plane : " << bitPlane << endl;
   return false;
}
//
//bool findNextComplex(unsigned char* block, bitmap_image* image) {
//	static int lastX = 0;
//	static int lastY = 0;unsigned char pixelBlock[kBlockSize][kBlockSize];
//	for (int x = lastX; x <= image->width() - kBlockSize; x += kBlockSize) {
//	   for (int y = lastY; y < image->height() - kBlockSize; y += kBlockSize) {
//		   for (int i = 0; i < kBlockSize; i++) {
//			   for (int j = 0; j < kBlockSize; j++) {
//				   image->get_pixel(x + i, y + j, red, green, blue);
//				   pixelBlock[i][j] = red;
//			   }
//		   }
//		   utility->extractBitPlane(pixelBlock, block, bitPlane);
//		   if (utility->isComplex(block)) {
//
//}

void imageToCGC(bitmap_image* image) {
   unsigned char red; // Red value of pixel
   unsigned char green; // Green value of pixel
   unsigned char blue; // Blue value of pixel
   CGCTranslator translator;
   for (size_t x = 0; x <= image->width(); x++) {
	   for (size_t y = 0; y < image->height(); y++) {
		   image->get_pixel(x, y, red, green, blue);
		   image->set_pixel(x, y, translator.binaryToCgc(red),
				   translator.binaryToCgc(green), translator.binaryToCgc(blue));
	   }
   }
}

void imageToPBC(bitmap_image* image) {
   unsigned char red; // Red value of pixel
   unsigned char green; // Green value of pixel
   unsigned char blue; // Blue value of pixel
   CGCTranslator translator;
   for (size_t x = 0; x <= image->width(); x++) {
	   for (size_t y = 0; y < image->height(); y++) {
		   image->get_pixel(x, y, red, green, blue);
		   image->set_pixel(x, y, translator.cgcToBinary(red),
				   translator.cgcToBinary(green), translator.cgcToBinary(blue));
	   }
   }
}

void decode() {
	// Open bitmap file
   bitmap_image stego(coverFileName);

   if (!stego)
   {
	  cout << "Error: Unable to open cover image file: "
			  << coverFileName << endl;
	  exit(1);
   }

   unsigned char red;
   unsigned char green;
   unsigned char blue;
   int color = kRed;
   unsigned char pixelBlock[kBlockSize][kBlockSize];
   unsigned char block[kBlockSize];
   int bitPlane = 0;
   for (int x = 0; x <= stego.width() - kBlockSize; x += kBlockSize) {
	   for (int y = 0; y < stego.height() - kBlockSize; y += kBlockSize) {

	   }
   }

}
