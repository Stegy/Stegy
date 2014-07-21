/*
 * BlockUtility.cpp
 *
 *  Created on: July 14, 2014
 *      Author: Joseph Alvizo Mendoza
 */

#include "stdio.h"
#include "BlockUtility.h"

#include <iostream>

using namespace std;

// BlockUtility constructor
BlockUtility::BlockUtility()
{
    threshold = .6; //Sets the default threshold
}

// Sets the Threshold to check for complexity
void BlockUtility::setComplexityThreshold(float n)
{
    threshold = n;
}

// Returns the Threshold for checking complexity
float BlockUtility::getComplexityThreshold()
{
    return threshold;
}

// Returns the complexity of a group of 8 unsigned chars
// Using the boarder complexity measurmant
float BlockUtility::getComplexity(unsigned char * block)
{
//    int intBlock[8][8];
//    bitset<8> blockBits[8];
//    for (int i = 0; i < 8; i++) {
//    	blockBits[i]
//    }
    unsigned char current;
    unsigned char last;
    unsigned int totalChanges = 0;
    unsigned int maxTotalChanges=0;
    float complexity;
    
    
    //checking for the number of changes
    for(x = 1; x < 8; x++)//in the x direction
    {
        for(y = 0; y < 8; y++)
        {
        	current = getBitOfChar(block[x], y);
        	last = getBitOfChar(block[x-1], y);
            if(last != current)
            {
                totalChanges++;
            }
            maxTotalChanges++;
        }
    }
    
    //checking for the number of changes
    for(y=1; y<8; y++)//in the y direction
    {
        for(x=0;x<8;x++)
        {
        	current = getBitOfChar(block[x], y);
        	last = getBitOfChar(block[x], y - 1);
            if(last!=current)
            {
                totalChanges++;
            }
            maxTotalChanges++;
        }
    }
    complexity = (float) totalChanges / (float) maxTotalChanges;
    return complexity;
}

// Returns if complexity of a group of 8 unsigned chars
// is higher then the threshold
// Using the boarder complexity measurmant
bool BlockUtility::isComplex(unsigned char * block)
{
////    int intBlock[8][8];
//    unsigned char current;
//    unsigned char last;
//    unsigned int totalChanges;
//    unsigned int maxTotalChanges;
//    float complexity;
//
//
//    //checking for the number of changes
//    for(x=1;x<8;x++)//in the x direction
//    {
//        for(y=0;y<8;y++)
//        {
//        	current = getBitOfChar(block[x], y);
////            current = intBlock[x][y];
//        	last = getBitOfChar(block[x-1], y);
////            last = intBlock[x-1][y];
//            if(last!=current)
//            {
//                totalChanges++;
//            }
//            maxTotalChanges++;
//        }
//    }
//
//    //checking for the number of changes
//    for(y=1;y<8;y++)//in the y direction
//    {
//        for(x=0;x<8;x++)
//        {
//        	//            current = intBlock[x][y];
//        	current = getBitOfChar(block[x], y);
//        	//            last = intBlock[x][y-1];
//        	current = getBitOfChar(block[x-1], y);
//        	if(last!=current)
//            {
//                totalChanges++;
//            }
//            maxTotalChanges++;
//        }
//    }
    float complexity = getComplexity(block);
    return (complexity >= threshold);
//    {
//        return true;
//    }
//    else
//    {
//        return 0;
//    }
}

// Returns a checkerboard pattern xored with the 8 unsigned chars
void BlockUtility::conjugate(unsigned char * block)
{
    unsigned char checkerBoard[] = {170,85,170,85,170,85,170,85};
    
    //Make conjugated array
    for(x=0;x<8;x++)
    {
        block[x] = checkerBoard[x]^block[x];
    }
}


// Prints out the array of 8 chars into a bit plane 1/0 representation
void BlockUtility::printBitPlane(unsigned char * block)
{
    unsigned char byte;
    //Print out bits of the char array
    for(x=0;x<8;x++)
    {
        byte = block[x];
        for(y=7;0<=y;y--)
        {
            printf("%d ",(byte >> y) & 0x01);
        }
        printf("\n");
    }

    cout << "USING BIT SETS:" << endl;
    for (int i = 0; i < 8; i++ ) {
    	bitset<8> x(block[i]);
    	cout << x << endl;
    }

}

// Prints out the plane of 8x8 chars
void BlockUtility::printValueBlock(unsigned char valueBlock[8][8])
{
    int x,y;
    for(x=0;x<8;x++)
    {
        for(y=0;y<8;y++)
        {
            printf("%d ",valueBlock[x][y]);
        }
        printf("\n");
    }
}

// Changes a array of 8x8 char to an array of 8 chars that represent the bit plane
// depending on the plane(int plane) is choosen
void BlockUtility::extractBitPlane(unsigned char valueBlock[8][8],
		unsigned char * byte, int plane)
{
    int x;
    int y;
    for(x = 0; x < 8; x++)
    {
        for(y = 0; y < 8; y++)
        {
            byte[x] = changeBitOfByte(byte[x], y,
            		getBitOfChar(valueBlock[x][7-y], plane));
        }
    }
}


// Changes the bit in a byte to the value
unsigned char BlockUtility::changeBitOfByte(unsigned char byte, int bit, int value)
{
    unsigned char output = byte;
    if(value)
        output |= 1 << bit;
    else
        output &= ~(1 << bit);
    
    return output;
}


// Returns the value of a bit from the char.
unsigned char BlockUtility::getBitOfChar(unsigned char byte, int bit)
{
    unsigned char output;
    output = ((byte >> bit) & 0x01);
    return output;
}

// Embeds the block bit plane into the plane oof the valueBlock;
void BlockUtility::embedBitPlane(unsigned char valueBlock[8][8], unsigned char block[8], int plane)
{
    int x,y;
    
    for(x=0;x<8;x++)
    {
        for(y=0;y<8;y++)
        {
            valueBlock[x][y] = changeBitOfByte(valueBlock[x][y],plane,getBitOfChar(block[x],7-y));
        }
    }
}
