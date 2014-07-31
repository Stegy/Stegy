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

BlockUtility::BlockUtility()
{
    threshold = .3;
}

void BlockUtility::conjugate(unsigned char* block)
{
    unsigned char checkerBoard[] = {170,85,170,85,170,85,170,85};
    int x;

    // Make conjugated array
    for(x = 0; x < 8; x++)
    {
        block[x] = checkerBoard[x]^block[x];
    }
}

void BlockUtility::embedBitPlane(unsigned char valueBlock[8][8],
		unsigned char block[8], int plane)
{
    int x,y;

    for(x = 0; x < 8; x++)
    {
        for(y = 0; y < 8; y++)
        {
            valueBlock[x][y] =
            		changeBitOfByte(valueBlock[x][y], plane,
            				getBitOfChar(block[x], 7-y));
        }
    }
}

void BlockUtility::extractBitPlane(unsigned char valueBlock[8][8],
		unsigned char* byte, int plane)
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

float BlockUtility::getComplexity(unsigned char * block)
{
    unsigned char current;
    unsigned char last;
    unsigned int totalChanges = 0;
    unsigned int maxTotalChanges=0;
    float complexity;
    int x;
    int y;
    
    // Checking for the number of changes
    for(x = 1; x < 8; x++) // In the x direction
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
    
    // Checking for the number of changes
    for(y=1; y<8; y++) // In the y direction
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

float BlockUtility::getComplexityThreshold()
{
    return threshold;
}

bool BlockUtility::isComplex(unsigned char * block)
{
    float complexity = getComplexity(block);
    return (complexity >= threshold);
}

void BlockUtility::printBitPlane(unsigned char * block)
{
    unsigned char byte;
    int x;
    int y;

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
}

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

void BlockUtility::setComplexityThreshold(float n)
{
    threshold = n;
}

unsigned char BlockUtility::changeBitOfByte(unsigned char byte, int bit,
		int value)
{
    unsigned char output = byte;
    if(value)
        output |= 1 << bit;
    else
        output &= ~(1 << bit);
    
    return output;
}

unsigned char BlockUtility::getBitOfChar(unsigned char byte, int bit)
{
    unsigned char output;
    output = ((byte >> bit) & 0x01);
    return output;
}
