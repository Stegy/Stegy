#include <iostream>
#include "BlockUtility.h"

using namespace std;

int main()
{
    BlockUtility Utility;
    unsigned char block[] = {20,100,211,43,255,222,76,150};
    unsigned char valueBlock[8][8]  = {
                                        {8,4,4,4,4,4,4,8},
                                        {4,4,4,4,4,4,4,4},
                                        {4,4,4,4,4,4,4,3},
                                        {4,4,4,4,4,4,4,4},
                                        {4,4,4,4,4,4,4,4},
                                        {4,4,4,4,4,4,4,4},
                                        {4,4,4,4,4,4,4,4},
                                        {6,4,4,4,4,4,4,2}
                                    };

    
    float complexity;
    int isComplex,x;
    unsigned char testByte;
    
    Utility.setComplexityThreshold(.4);
    Utility.printBitPlane(block);
    complexity = Utility.getComplexity(block);
    isComplex =  Utility.isComplex(block);
    printf("Is complex: %d\n",isComplex);
    printf("Complexity: %0.3f\n",complexity);
    
    cout << "TEST ZEROES" << endl;
    unsigned char testZeroes[8] = {0};
    Utility.printBitPlane(testZeroes);
    float alpha = Utility.getComplexity(testZeroes);
    cout << "Complexity: " << alpha << endl;

    printf("\n\nAfter conjugation\n");
    Utility.conjugate(block);
    Utility.printBitPlane(block);
    complexity = Utility.getComplexity(block);
    isComplex =  Utility.isComplex(block);
    printf("Is complex: %d\n",isComplex);
    printf("Complexity: %0.3f\n",complexity);
    

    testByte = (unsigned char)32;
    printf("\nTesting changeBitOfByte:\n");
    printf("testByte Before: %d\n",testByte);
    testByte = Utility.changeBitOfByte(testByte,0,1);
    printf("testByte after: %d\n\n",testByte);
    
    testByte = (unsigned char) 87;
    printf("\nTesting getBitOfChar:\n");
    printf("testByte Before: %d\n",testByte);
    testByte = Utility.getBitOfChar(testByte,8);
    printf("testByte after: %d\n",testByte);
    
    printf("\n\nTesting printing of planes\n");
    printf("==========================\n");
    printf("Value block:\n");
    Utility.printValueBlock(valueBlock);
    for(x=0;x<8;x++)
    {
        Utility.extractBitPlane(valueBlock,block, x);
        printf("Bit Plane %d:\n",x);
        Utility.printBitPlane(block);  
    }
    
    
    unsigned char testBlock[8] = {15,15,15,15,15,15,15,15};
    printf("\n\nTesting the Embeding of a bit plane in a color value Plane\n");
    printf("==========================\n");
    printf("Cover 8x8 reb pixels values:\n");
    Utility.printValueBlock(valueBlock);
    printf("Message to be inbedded:\n");
    Utility.printBitPlane(testBlock);
    printf("Now with message embedded in the cover:\n");
    Utility.embedBitPlane(valueBlock, testBlock, 2);
    Utility.printValueBlock(valueBlock);
}
