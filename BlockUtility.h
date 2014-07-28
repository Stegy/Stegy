/*
 * BlockUtility.h
 *
 * Contains the BlockUtility class definition.
 *
 *  Created on: July 14, 2014
 *      Author: Joseph Alvizo Mendoza
 */

#ifndef __BlockUtility_H_INCLUDED__
#define __BlockUtility_H_INCLUDED__

#pragma once

class BlockUtility
{
	public:
		/**
		 * Constructor sets the default threshold to 0.3.
		 */
		BlockUtility();
		void conjugate(unsigned char *);
		void embedBitPlane(unsigned char[8][8], unsigned char[8], int);
		void extractBitPlane(unsigned char[8][8],unsigned char[8], int);
		float getComplexity(unsigned char *);
		float getComplexityThreshold();
		bool isComplex(unsigned char *);
		void printBitPlane(unsigned char *);
		void printValueBlock(unsigned char[8][8]);
		void setComplexityThreshold(float);

	private:
		unsigned char changeBitOfByte(unsigned char, int , int);
		unsigned char getBitOfChar(unsigned char, int);
        float threshold;
};

#endif /* __BlockUtility_H_INCLUDED__ */
