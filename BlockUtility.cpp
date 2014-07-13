#include "stdio.h"
#include "BlockUtility.h"

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
    int intBlock[8][8];
    unsigned char current;
    unsigned char last;
    unsigned int totalChanges = 0;
    unsigned int maxTotalChanges=0;
    float complexity;
    
    
    //checking for the number of changes
    for(x=1;x<8;x++)//in the x direction
    {
        for(y=0;y<8;y++)
        {
            current = intBlock[x][y];
            last = intBlock[x-1][y];
            if(last!=current)
            {
                totalChanges++;
            }
            maxTotalChanges++;
        }
    }
    
    //checking for the number of changes
    for(y=1;y<8;y++)//in the y direction
    {
        for(x=0;x<8;x++)
        {
            current = intBlock[x][y];
            last = intBlock[x][y-1];
            if(last!=current)
            {
                totalChanges++;
            }
            maxTotalChanges++;
        }
    }
    complexity = (float)totalChanges/(float)maxTotalChanges;
    return complexity;
}

// Returns if complexity of a group of 8 unsigned chars
// is higher then the threshold
// Using the boarder complexity measurmant
int BlockUtility::isComplex(unsigned char * block)
{
    int intBlock[8][8];
    unsigned char current;
    unsigned char last;
    unsigned int totalChanges;
    unsigned int maxTotalChanges;
    float complexity;
    
    
    //checking for the number of changes
    for(x=1;x<8;x++)//in the x direction
    {
        for(y=0;y<8;y++)
        {
            current = intBlock[x][y];
            last = intBlock[x-1][y];
            if(last!=current)
            {
                totalChanges++;
            }
            maxTotalChanges++;
        }
    }
    
    //checking for the number of changes
    for(y=1;y<8;y++)//in the y direction
    {
        for(x=0;x<8;x++)
        {
            current = intBlock[x][y];
            last = intBlock[x][y-1];
            if(last!=current)
            {
                totalChanges++;
            }
            maxTotalChanges++;
        }
    }
    complexity = (float)totalChanges/(float)maxTotalChanges;
    if(complexity>=threshold)
    {
        return 1;
    }
    else
    {
        return 0;
    }
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
}