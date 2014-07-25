#include "bitmap_image.hpp"
#include "BlockUtility.h"
#include "CGCTranslator.h"
#include "MessageReader.h"
#include "MessageWriter.h"
#include "StegoCommon.h"

#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

/* Command line arguments */
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

/* Global variables */
BlockUtility* utility; // Used to test complexity and conjugate blocks
MessageReader* reader; // Used to read secret message for embedding
MessageWriter* writer; // Used to write secret message when decoding
CGCTranslator* cgc; // Translates bytes between PBC and CGC

int blockIndex = 0; // Current block index for embedding/decoding
//int messageSize = 0; // Size of secret message
bool sizeFound = false;
int numMapBlocks = -1;
int mapBlocksFound = 0;

/* Used as constants for red, green, and blue */
int kRed = 0;
int kGreen = 1;
int kBlue = 2;

/* Used to reserve space for map blocks when embedding */
struct MapBlockCoords {
	pair<int, int> pixelCorner;
	int bitPlane;
	int color;
};
vector<MapBlockCoords> mapBlockCoords;
int conjCount = 0;

void verifyArguments(int argc, char* argv[]);
void encode();
void decode();
bool traverseBitPlaneEmbed(int bitPlane, int color, bitmap_image* image);
bool traverseBitPlaneDecode(int bitPlane, int color, bitmap_image* image);
void imageToCGC(bitmap_image* image);
void imageToPBC(bitmap_image* image);
void embedMapBlocks(bitmap_image* image);
void readPixelBlock(bitmap_image* image, unsigned char redValues[8][8],
		unsigned char blueValues[8][8], unsigned char greenValues[8][8],
		int startX, int startY);
void writePixelBlock(bitmap_image* image, unsigned char redValues[8][8],
		unsigned char greenValues[8][8], unsigned char blueValues[8][8],
		int startX, int startY);


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

   // Translate cover image from BPC to CGC
   imageToCGC(&cover);

   // Traverse bit planes from lowest to highest and embed message
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
   if (!done) {
	   cout << "Unable to embed entire message" << endl;
   }
   // Embed the completed map blocks in the image
   embedMapBlocks(&cover);
   // Translate image back to PBC
   imageToPBC(&cover);
   // Write bitmap with embedded data (the stego image) to file
   cover.save_image(outputFileName);
}

// Returns whether message ended
bool traverseBitPlaneEmbed(int bitPlane, int color, bitmap_image* image) {
   unsigned char red; // Red value of single pixel
   unsigned char green; // Green value of single pixel
   unsigned char blue; // Blue value of single pixel
   // Color values for 8x8 pixel blocks
   unsigned char redPixelValues[kBlockSize][kBlockSize];
   unsigned char greenPixelValues[kBlockSize][kBlockSize];
   unsigned char bluePixelValues[kBlockSize][kBlockSize];
   // 8x8 block from a single bit plane
   unsigned char block[kBlockSize];
   // Traverse image pixels
   int width = image->width();
   int height = image->height();
   bool done = false;
   int tmpCount = 0;
   for (int x = 0; x <= width - kBlockSize; x += kBlockSize) {
	   for (int y = 0; y < height - kBlockSize; y += kBlockSize) {
		   // Get color values for 8x8 pixel blocks
		   readPixelBlock(image, redPixelValues, greenPixelValues,
				   bluePixelValues, x, y);
		   // Get desired bit plane
		   if (redFlag) {
			   utility->extractBitPlane(redPixelValues, block, bitPlane);
		   } else if (greenFlag) {
			   utility->extractBitPlane(greenPixelValues, block, bitPlane);
		   } else {
			   utility->extractBitPlane(bluePixelValues, block, bitPlane);
		   }
		   if (utility->isComplex(block)) {
			   if (sizeFound && (mapBlocksFound < numMapBlocks)) {
				   // Reserve space for map blocks
				   // Applies to complex blocks 1 -> number of map blocks
				   MapBlockCoords mbc;
				   mbc.pixelCorner.first = x;
				   mbc.pixelCorner.second = y;
				   mbc.bitPlane = bitPlane;
				   mbc.color = color;
				   mapBlockCoords.push_back(mbc);
				   mapBlocksFound++;
			   } else {
				   if (!sizeFound) {
					   // First block to embed in - will embed the size block
					   reader->getSizeBlock(block);
					   numMapBlocks = reader->getNumMapBlocks();
					   sizeFound = true;
					   cout << "Size block: " << endl;
					   utility->printBitPlane(block);
				   }  else {
					   // Will embed a normal message block
					   done = reader->getNext(kBlockSize, block);
					   // Conjugate block if necessary, setting map value
					   if (!utility->isComplex(block)) {
						   cout << "Conjugated block" << endl;
						   utility->conjugate(block);
						   reader->setMapBit(blockIndex, true);
					   }
					   blockIndex++;
				   }
				   // Modify the current block of color values to contain the
				   // secret block
				   cout << "Embedding message block: " << blockIndex - 1 << "at " << x << ", " << y << endl;
				   utility->printBitPlane(block);
				   cout << "Complexity of embedded block: " << blockIndex - 1 << " " << utility->getComplexity(block) << endl;
				   utility->embedBitPlane(redPixelValues, block, bitPlane);
				   // Set the color values in the 8x8 pixel block of the bitmap
				   for (size_t i = 0; i < kBlockSize; i++) {
					   for (size_t j = 0; j < kBlockSize; j++) {
						   image->set_pixel(x + i, y + j,
								   redPixelValues[i][j],
								   greenPixelValues[i][j],
								   bluePixelValues[i][j]);
					   }
				   }
				   tmpCount++;
				   if (done) {
					   cout << "Done with message, size: "
							   << reader->getSize() << endl;
					   cout << "bitplane: " << bitPlane << endl;
					   cout << "Embedded blocks: " << tmpCount << endl;
					   return true;
				   }
			   }
		   }
	   }
   }
   cout << "finished bit plane : " << bitPlane << endl;
   return false;
}

void embedMapBlocks(bitmap_image* image) {
	// Color values for 8x8 pixel blocks
	unsigned char redPixelValues[kBlockSize][kBlockSize];
	unsigned char greenPixelValues[kBlockSize][kBlockSize];
	unsigned char bluePixelValues[kBlockSize][kBlockSize];
	unsigned char block[kBlockSize];
	for (int i = 0; i < mapBlockCoords.size(); i++) {
		MapBlockCoords mbc = mapBlockCoords[i];
		int x = mbc.pixelCorner.first;
		int y = mbc.pixelCorner.second;
		// Read pixel block at correct location
		readPixelBlock(image, redPixelValues, greenPixelValues, bluePixelValues,
				x, y);
		// Get the map block to hide
		reader->getNextMapBlock(block);
		cout << "Hid map block at : " << x << ", " << y << endl;
		utility->printBitPlane(block);
		// Hide in the reserved bit plane in the given color
		if (mbc.color == kRed) {
			utility->embedBitPlane(redPixelValues, block, mbc.bitPlane);
		} else if (mbc.color == kGreen) {
			utility->embedBitPlane(greenPixelValues, block, mbc.bitPlane);
		} else {
			utility->embedBitPlane(bluePixelValues, block, mbc.bitPlane);
		}
//		utility->extractBitPlane(redPixelValues, block, mbc.bitPlane);
		writePixelBlock(image, redPixelValues, greenPixelValues,
				bluePixelValues, x, y);
	}
}

void readPixelBlock(bitmap_image* image, unsigned char redValues[8][8],
		unsigned char greenValues[8][8], unsigned char blueValues[8][8],
		int startX, int startY) {
   unsigned char red; // Red value of single pixel
   unsigned char green; // Green value of single pixel
   unsigned char blue; // Blue value of single pixel
   for (size_t i = 0; i < kBlockSize; i++) {
	   for (size_t j = 0; j < kBlockSize; j++) {
		   image->get_pixel(startX + i, startY + j, red, green, blue);
		   redValues[i][j] = red;
		   greenValues[i][j] = green;
		   blueValues[i][j] = blue;
	   }
   }
}

void writePixelBlock(bitmap_image* image, unsigned char redValues[8][8],
		unsigned char greenValues[8][8], unsigned char blueValues[8][8],
		int startX, int startY) {
   for (size_t i = 0; i < kBlockSize; i++) {
	   for (size_t j = 0; j < kBlockSize; j++) {
		   image->set_pixel(startX + i, startY + j, redValues[i][j],
				   greenValues[i][j], blueValues[i][j]);
	   }
   }
}

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
   bitmap_image stego(stegoFileName);
   if (!stego)
   {
	  cout << "Error: Unable to open cover image file: "
			  << stegoFileName << endl;
	  exit(1);
   }

   imageToCGC(&stego);

   // Open output file for writing message
   writer = new MessageWriter(outputFileName, utility);
   // TODO check for errors
   bool done = false;
   for (int i = 0; i < 8 && !done; i++) {
	   if (redFlag || allFlag) {
		   done = traverseBitPlaneDecode(i, kRed, &stego);
	   }
	   if (greenFlag || allFlag) {
		   done = traverseBitPlaneDecode(i, kGreen, &stego);
	   }
	   if (blueFlag || allFlag) {
		   done = traverseBitPlaneDecode(i, kBlue, &stego);
	   }
   }
}

bool traverseBitPlaneDecode(int bitPlane, int color, bitmap_image* image) {
   unsigned char red; // Red value of single pixel
   unsigned char green; // Green value of single pixel
   unsigned char blue; // Blue value of single pixel
   // Color values for 8x8 pixel blocks
   unsigned char redPixelValues[kBlockSize][kBlockSize];
   unsigned char greenPixelValues[kBlockSize][kBlockSize];
   unsigned char bluePixelValues[kBlockSize][kBlockSize];
   // 8x8 block from a single bit plane
   unsigned char block[kBlockSize];
   // Traverse image pixels
   int width = image->width();
   int height = image->height();
   for (int x = 0; x <= width - kBlockSize; x += kBlockSize) {
	   for (int y = 0; y < height - kBlockSize; y += kBlockSize) {
		   // Get color values for 8x8 pixel blocks
		   readPixelBlock(image, redPixelValues, greenPixelValues,
				   bluePixelValues, x, y);
		   // Get desired bit plane
		   if (redFlag) {
			   utility->extractBitPlane(redPixelValues, block, bitPlane);
		   } else if (greenFlag) {
			   utility->extractBitPlane(greenPixelValues, block, bitPlane);
		   } else {
			   utility->extractBitPlane(bluePixelValues, block, bitPlane);
		   }
		   // If complex block is found, decode the message value
		   if (utility->isComplex(block)) {
			   if (!sizeFound) {
				   // First block holds the size
				   cout << "Foudn size block: " << endl;
				   utility->printBitPlane(block);
				   writer->decodeSizeBlock(block);
				   sizeFound = true;
				   numMapBlocks = writer->getNumMapBlocks();
			   } else if (sizeFound && mapBlocksFound < numMapBlocks) {
				   // Next few blocks hold the conjugation map
				   cout << "Found map block: " << endl;
				   utility->printBitPlane(block);
				   writer->decodeNextMapBlock(block);
				   mapBlocksFound++;
			   } else {
				   // Normal message block read
				   cout << "Found message block: " << blockIndex << " at " << x << ", " << y << endl;
				   utility->printBitPlane(block);
				   bool done = writer->decodeNextMessageBlock(block, blockIndex);
				   if (done) {
					   cout << "Finished decoding message of size "
							   << writer->getMessageSize() << endl;
					   writer->closeFile();
					   return true;
				   }
				   blockIndex++;
			   }
		   }
	   }
   }
   cout << "finished bit plane : " << bitPlane << endl;
   return false;
}
