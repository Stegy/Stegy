/*
 * BlockUtility.h
 *
 *  Created on: July 14, 2014
 *      Author: Joseph Alvizo Mendoza
 */

#define __BlockUtility_H_INCLUDED__   //   #define this so the compiler knows it has been included
class BlockUtility
{
   public:
        BlockUtility();
        void conjugate(unsigned char *);
        void setComplexityThreshold(float);
        float getComplexityThreshold();
        float getComplexity(unsigned char *);
        int isComplex(unsigned char *);
        void printBitPlane(unsigned char *);
        void printValueBlock(unsigned char[8][8]);
        void valueToCharArray(unsigned char[8][8],unsigned char[8], int);
        unsigned char changeBitOfByte(unsigned char, int , int);
        unsigned char getBitOfChar(unsigned char, int);
        void embedIntoValueBlock(unsigned char[8][8], unsigned char[8], int);
    private:
        float threshold;
        int x;
        int y;
};