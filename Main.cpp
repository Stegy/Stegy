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
int colorDepth = 24;
string coverFileName = "";
string messageFileName = "";
string stegoFileName = "";
string outputFileName = "";

/* Global variables */
BlockUtility* utility = NULL; // Used to test complexity and conjugate blocks
MessageReader* reader = NULL; // Used to read secret message for embedding
MessageWriter* writer = NULL; // Used to write secret message when decoding
CGCTranslator* cgc = NULL; // Translates bytes between PBC and CGC

int blockIndex = 0; // Current index of message block for embedding/decoding
// Whether the message size block has been embedded/decoded
bool sizeFound = false;
// Total number of conjugation map blocks associated with secret message
int numMapBlocks = -1;
int mapBlocksFound = 0; // Number of conjugation map blocks embedded/decoded
int complexBlockCount = 0; // Count of complex blocks (for capacity analysis)

// Buffers for color values of 8x8 pixel blocks
unsigned char redPixelValues[kBlockSize][kBlockSize];
unsigned char greenPixelValues[kBlockSize][kBlockSize];
unsigned char bluePixelValues[kBlockSize][kBlockSize];
// Buffer for 8x8 block from a single bit plane
unsigned char block[kBlockSize];

/* Used as constants for red, green, and blue */
int kRed = 0;
int kGreen = 1;
int kBlue = 2;

/* Used to reserve space for map blocks when embedding */
struct MapBlockCoords {
	pair<int, int> pixelCorner; // Minimum x and minimum y of the 8x8 block
	int bitPlane; // Bit plane from 0->7 (lowest->highest)
	int color; // Color constant value
};
vector<MapBlockCoords> mapBlockCoords; // Holds info for all map blocks

/* Methods used in Main (see comments below) */
void decode();
void embed();
void embedMapBlocks(bitmap_image* image);
void imageToCGC(bitmap_image* image);
void imageToPBC(bitmap_image* image);
void performAnalysis();
void readPixelBlock(bitmap_image* image, unsigned char redValues[8][8],
		unsigned char blueValues[8][8], unsigned char greenValues[8][8],
		int startX, int startY);
bool traverseBitPlaneDecode(int bitPlane, int color, bitmap_image* image);
bool traverseBitPlaneEmbed(int bitPlane, int color, bitmap_image* image);
void traverseForAnalysis(int bitPlane, int color, bitmap_image* image);
void verifyArguments(int argc, char* argv[]);
void writePixelBlock(bitmap_image* image, unsigned char redValues[8][8],
		unsigned char greenValues[8][8], unsigned char blueValues[8][8],
		int startX, int startY);


/* Main method - overall program control */
int main(int argc, char* argv[])
{
    verifyArguments(argc, argv);
    utility = new BlockUtility();
    utility->setComplexityThreshold(complexityTH);
    cgc = new CGCTranslator();
    if (encodeFlag) {
    	embed();
    } else if (decodeFlag) {
    	decode();
    } else { // Analysis
    	performAnalysis();
    }
    delete utility;
    if (reader != NULL) {
    	delete reader;
    }
    if (writer != NULL) {
    	delete writer;
    }
    if (cgc != NULL) {
    	delete cgc;
    }
}

/**
 * Verifies the command line arguments. If the given arguments are not correct,
 * it prints out a usage message and exits the program.
 */
void verifyArguments(int argc, char* argv[])
{
    string usage =  "\nstegy -a -c alpha  < -red | -green | -blue | -all > "
    		"-cover CoverImage -depth Depth\nstegy -e -c alpha  < -red | -green | -blue | "
    		"-all > -cover CoverImage -depth Depth -message Message -o OutputFileName\n"
    		"stegy -d -c alpha  < -red | -green | -blue | -all > -stego "
    		"StegoImage -depth Depth -o OutputFileName\n";

    if(argc < 5)
    {
        cout << "Usage: " << usage << endl;
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
                cout << "Usage: " << usage << endl;
                exit(1);
            }
            try
            {
                complexityTH = atof(argv[i+1]);
            }
            catch(...)
            {
                cout << "Invalid Complexity : " << argv[i+1] << endl;
                cout << "Usage: " << usage << endl;
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
                cout << "Usage: " << usage << endl;
                exit(1);
            }
            coverFileName = argv[i+1];
            i++;
        }
	else if(strcmp(argv[i],"-depth") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage: " << usage << endl;
                exit(1);
            }
            try
            {
                colorDepth = atoi(argv[i+1]);
            }
            catch(...)
            {
                cout << "Invalid Color Depth : " << argv[i+1] << endl;
                cout << "Usage: " << usage << endl;
                exit(1);
            }
            i++;
        }
        else if(strcmp(argv[i],"-stego") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage: " << usage << endl;
                exit(1);
            }
            stegoFileName = argv[i+1];
            i++;
        }
        else if(strcmp(argv[i], "-message") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage: " << usage << endl;
                exit(1);
            }
            messageFileName = argv[i+1];
            i++;
        }
        else if(strcmp(argv[i],"-o") == 0)
        {
            if(i+1 >= argc)
            {
                cout << "Usage: " << usage << endl;
                exit(1);
            }
            outputFileName = argv[i+1];
            i++;
        }
    }
    
    if(!(analysisFlag || encodeFlag || decodeFlag))
    {
    	cout << "Mode must be one of: -a (analysis), -e (encode) or "
    			"-d (decode)" << endl;
        cout << "Usage: " << usage << endl;
        exit(1);
    }
    if(!(redFlag || blueFlag || greenFlag || allFlag))
    {
    	cout << "Must include a color option: -red, -green, -blue, or -all"
    			<< endl;
        cout << "Usage: " << usage << endl;
        exit(1);
    }
    if (complexityTH == -1) {
    	cout << "Complexity value is required." << endl;
        cout << "Usage: " << usage << endl;
        exit(1);
    }
    if(complexityTH > 0.5 || complexityTH < 0)
    {
        cout << "Complexity Threshold must be between 0 and 0.5. Found value : "
        		<< complexityTH << endl;
        cout << "Usage: " << usage << endl;
        exit(1);
    }
    if(analysisFlag && coverFileName.empty())
    {
        cout << "Need file for analysis: (-cover <filename>)" << endl;
        cout << "Usage: " << usage << endl;
        exit(1);
    }
    if(encodeFlag && ( coverFileName.empty() || messageFileName.empty()
    		|| outputFileName.empty() ))
    {
        cout << "Need file names for cover image, message, and output" << endl;
        cout << "Usage: " << usage << endl;
        exit(1);
    }
    if(decodeFlag && ( stegoFileName.empty() || outputFileName.empty() ))
    {
        cout << "Need file names for stego image and output" << endl;
        cout << "Usage: " << usage << endl;
        exit(1);
    }
}

/**
 * Control for embedding the message.
 */
void embed() {
	// Open bitmap file for cover image - Partow library code
   bitmap_image cover(coverFileName);
   if (!cover)
   {
	  cout << "Error: Unable to open cover image file: "
			  << coverFileName << endl;
	  exit(1);
   }

   // Open message file
   reader = new MessageReader(messageFileName, utility);
   numMapBlocks = reader->getNumMapBlocks(); // Number of map blocks needed

   // Translate cover image from BPC to CGC
   imageToCGC(&cover);

   // Traverse bit planes from lowest to highest, in desired colors,
   // and embed message
   bool done = false;
   for (int i = 0; i < 8 && !done; i++) {
	   if (redFlag || allFlag) {
		   done = traverseBitPlaneEmbed(i, kRed, &cover);
	   }
	   if ((greenFlag || allFlag) && !done) {
		   done = traverseBitPlaneEmbed(i, kGreen, &cover);
	   }
	   if ((blueFlag || allFlag) && !done) {
		   done = traverseBitPlaneEmbed(i, kBlue, &cover);
	   }
   }
   if (!done) {
	   cout << "Unable to embed entire message" << endl;
	   cout << "Embedded " << reader->getCurrentSize() << " bytes" << endl;
   }
   // Embed the completed map blocks in the image
   embedMapBlocks(&cover);
   // Translate image back to PBC
   imageToPBC(&cover);
   // Write bitmap with embedded data (the stego image) to file
   cover.save_image(outputFileName);
}

/**
 * Traverses the given bit plane in the given color to embed the message. Finds
 * complex 8x8 blocks, and replaces them with secret message data. Returns true
 * if the entire message has been embedded, and false otherwise.
 */
bool traverseBitPlaneEmbed(int bitPlane, int color, bitmap_image* image) {
	int width = image->width();
	int height = image->height();
	bool done = false;
	// Traverse image pixels
	for (int x = 0; x <= width - kBlockSize; x += kBlockSize) {
	   for (int y = 0; y < height - kBlockSize; y += kBlockSize) {
		   // Get color values for 8x8 pixel blocks
		   readPixelBlock(image, redPixelValues, greenPixelValues,
				   bluePixelValues, x, y);
		   // Get desired bit plane
		   if (color == kRed) {
			   utility->extractBitPlane(redPixelValues, block, bitPlane);
		   } else if (color == kGreen) {
			   utility->extractBitPlane(greenPixelValues, block, bitPlane);
		   } else {
			   utility->extractBitPlane(bluePixelValues, block, bitPlane);
		   }
		   if (utility->isComplex(block)) {
			   if (sizeFound && (mapBlocksFound < numMapBlocks)) {
				   // Reserve space for map blocks after size block
				   MapBlockCoords mbc;
				   mbc.pixelCorner.first = x;
				   mbc.pixelCorner.second = y;
				   mbc.bitPlane = bitPlane;
				   mbc.color = color;
				   mapBlockCoords.push_back(mbc);
				   mapBlocksFound++;
			   } else {
				   if (!sizeFound) {
					   // Get and embed the size block first
					   reader->getSizeBlock(block);
					   sizeFound = true;
				   } else {
					   // Get and embed a normal message block, check if done
					   done = reader->getNext(block);
					   // Conjugate block if necessary, setting map value
					   if (!utility->isComplex(block)) {
						   utility->conjugate(block);
						   reader->setMapBit(blockIndex, true);
					   }
					   blockIndex++;
				   }
				   // Hide in the reserved bit plane in the given color
				   if (color == kRed) {
					   utility->embedBitPlane(redPixelValues, block, bitPlane);
				   } else if (color == kGreen) {
					   utility->embedBitPlane(greenPixelValues, block,
							   bitPlane);
				   } else {
					   utility->embedBitPlane(bluePixelValues, block, bitPlane);
				   }
				   // Set the color values in the 8x8 pixel block of the bitmap
				   writePixelBlock(image, redPixelValues, greenPixelValues,
						   bluePixelValues, x, y);
				   if (done) {
					   cout << "Done with message, size: "
							   << reader->getSize() << endl;
					   cout << "Reached bit plane: " << bitPlane << endl;
					   return true;
				   }
			   }
		   }
	   }
	}
	return false;
}

/**
 * Embeds the completed map blocks into the image in the places reserved for
 * them.
 */
void embedMapBlocks(bitmap_image* image) {
	// Iterate through all map block coordinates saved
	for (uint i = 0; i < mapBlockCoords.size(); i++) {
		MapBlockCoords mbc = mapBlockCoords[i];
		int x = mbc.pixelCorner.first;
		int y = mbc.pixelCorner.second;
		// Read pixel block at correct location
		readPixelBlock(image, redPixelValues, greenPixelValues, bluePixelValues,
				x, y);
		// Get the map block to hide
		reader->getNextMapBlock(block);
		// Hide in the reserved bit plane in the given color
		if (mbc.color == kRed) {
			utility->embedBitPlane(redPixelValues, block, mbc.bitPlane);
		} else if (mbc.color == kGreen) {
			utility->embedBitPlane(greenPixelValues, block, mbc.bitPlane);
		} else {
			utility->embedBitPlane(bluePixelValues, block, mbc.bitPlane);
		}
		// Write the values containing the hidden block into the image
		writePixelBlock(image, redPixelValues, greenPixelValues,
				bluePixelValues, x, y);
	}
}

/**
 * Reads an 8x8 pixel block starting at the given x, y coordinates. Saves the
 * values for red, green, and blue into 8x8 unsigned char arrays.
 */
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

/**
 * Writes an 8x8 pixel block to the image at the given x, y coordinates. Takes
 * 8x8 unsigned char arrays containing the red, green, and blue values to
 * set in the image at this point.
 */
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

/**
 * Translates the image from Pure Binary Code (PBC) to Canonical Gray Code
 * (CGC).
 */
void imageToCGC(bitmap_image* image) {
   unsigned char red; // Red value of pixel
   unsigned char green; // Green value of pixel
   unsigned char blue; // Blue value of pixel
   CGCTranslator translator;
   for (size_t x = 0; x < image->width(); x++) {
	   for (size_t y = 0; y < image->height(); y++) {
		   image->get_pixel(x, y, red, green, blue);
		   image->set_pixel(x, y, translator.pbcToCgc(red),
				   translator.pbcToCgc(green), translator.pbcToCgc(blue));
	   }
   }
}

/**
 * Translates the image from Canonical Gray Code (CGC) to Pure Binary Code
 * (PBC).
 */
void imageToPBC(bitmap_image* image) {
   unsigned char red; // Red value of pixel
   unsigned char green; // Green value of pixel
   unsigned char blue; // Blue value of pixel
   CGCTranslator translator;
   for (size_t x = 0; x < image->width(); x++) {
	   for (size_t y = 0; y < image->height(); y++) {
		   image->get_pixel(x, y, red, green, blue);
		   image->set_pixel(x, y, translator.cgcToPbc(red),
				   translator.cgcToPbc(green), translator.cgcToPbc(blue));
	   }
   }
}

/**
 * Control for decoding (extracting) a secret message from the stego image.
 */
void decode() {
	// Open bitmap file for stego image
   bitmap_image stego(stegoFileName);
   if (!stego)
   {
	  cout << "Error: Unable to open stego image file: "
			  << stegoFileName << endl;
	  exit(1);
   }

   // Translate the image to CGC
   imageToCGC(&stego);

   // Open output file for writing message
   writer = new MessageWriter(outputFileName, utility);

   // Traverse bit planes from lowest to highest, in desired colors,
   // and extract message
   bool done = false;
   for (int i = 0; i < 8 && !done; i++) {
	   if (redFlag || allFlag) {
		   done = traverseBitPlaneDecode(i, kRed, &stego);
	   }
	   if ((greenFlag || allFlag) && !done) {
		   done = traverseBitPlaneDecode(i, kGreen, &stego);
	   }
	   if ((blueFlag || allFlag) && !done) {
		   done = traverseBitPlaneDecode(i, kBlue, &stego);
	   }
   }
   if (!done) {
	   cout << "Reached the end before finding entire message." << endl;
	   cout << "Bytes read: " << writer->getCurrentSize();
	   writer->closeFile();
   }
}

/**
 * Traverses the given bit plane in the given color to extract the message.
 * Finds complex 8x8 blocks, and gets the size, conjugation map, and secret
 * message binary data from these blocks. Returns true if the entire message
 * has been discovered, and false otherwise.
 */
bool traverseBitPlaneDecode(int bitPlane, int color, bitmap_image* image) {
   // Traverse image pixels
   int width = image->width();
   int height = image->height();
   for (int x = 0; x <= width - kBlockSize; x += kBlockSize) {
	   for (int y = 0; y < height - kBlockSize; y += kBlockSize) {
		   // Get color values for 8x8 pixel blocks
		   readPixelBlock(image, redPixelValues, greenPixelValues,
				   bluePixelValues, x, y);
		   // Get desired bit plane
		   if (color == kRed) {
			   utility->extractBitPlane(redPixelValues, block, bitPlane);
		   } else if (color == kGreen) {
			   utility->extractBitPlane(greenPixelValues, block, bitPlane);
		   } else {
			   utility->extractBitPlane(bluePixelValues, block, bitPlane);
		   }
		   // If complex block is found, decode the message value
		   if (utility->isComplex(block)) {
			   if (!sizeFound) {
				   // First block holds the size
				   writer->decodeSizeBlock(block);
				   sizeFound = true;
				   // Get number of map blocks after finding the size
				   numMapBlocks = writer->getNumMapBlocks();
			   } else if (sizeFound && mapBlocksFound < numMapBlocks) {
				   // Next few blocks hold the conjugation map
				   writer->decodeNextMapBlock(block);
				   mapBlocksFound++;
			   } else {
				   // Normal message block read
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
   return false;
}

/**
 * Control for getting analysis information from image.
 */
void performAnalysis() {

	// Open image
	bitmap_image image(coverFileName);
	if (!image) {
		cout << "Error: Unable to open cover image file: "
				<< coverFileName << endl;
		exit(1);
	}
	// Traverse desired bit planes, finding complexities and complex block count
	for (int i = 0; i < 8; i++) {
		if (redFlag || allFlag) {
			traverseForAnalysis(i, kRed, &image);
		}
		if (greenFlag || allFlag) {
			traverseForAnalysis(i, kGreen, &image);
		}
		if (blueFlag || allFlag) {
			traverseForAnalysis(i, kBlue, &image);
		}
	}
	cout << "\n\n\nComplex blocks found: " << complexBlockCount << endl;
}

/**
 * Traverses the given bit plane to find information for analysis. Prints out
 * the complexity of each 8x8 block, and keeps a count of complex blocks found
 * using the given threshold.
 */
void traverseForAnalysis(int bitPlane, int color, bitmap_image* image) {
   int width = image->width();
   int height = image->height();
   cout << "Bit plane: " << bitPlane << " Color: " << color << endl;
   for (int x = 0; x <= width - kBlockSize; x += kBlockSize) {
	   for (int y = 0; y < height - kBlockSize; y += kBlockSize) {
		   // Get color values for 8x8 pixel blocks
		   readPixelBlock(image, redPixelValues, greenPixelValues,
				   bluePixelValues, x, y);
		   // Get desired bit plane
		   if (color == kRed) {
			   utility->extractBitPlane(redPixelValues, block, bitPlane);
		   } else if (color == kGreen) {
			   utility->extractBitPlane(greenPixelValues, block, bitPlane);
		   } else {
			   utility->extractBitPlane(bluePixelValues, block, bitPlane);
		   }
		   // Print complexity of block
		   cout << utility->getComplexity(block) << endl;
		   // Count complex blocks
		   if (utility->isComplex(block)) {
			   complexBlockCount++;
		   }
	   }
   }
}
