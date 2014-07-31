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

		/**
		 * Conjugates the given 8x8 block. XORs the block with a "checkerboard"
		 * 8x8 pattern (starting with 1 in the upper lefthand corner).
		 */
		void conjugate(unsigned char* block);

		/**
		 * Embeds the bit plane "block" into the given plane of the 8x8
		 * valueBlock. Here, valueBlock contains the values of a single
		 * color in an 8x8 pixel block from the bitmap image. "Plane" refers to
		 * the bit plane, where 0 is the lowest, and 7 is the highest.
		 */
		void embedBitPlane(unsigned char valueBlock[8][8],
				unsigned char block[8], int plane);

		/**
		 * Extracts the desired bit plane (plane) from the valueBlock. Value
		 * block contains the values of a single color from an 8x8 pixel block.
		 * The extracted bit plane is stored in "byte."
		 */
		void extractBitPlane(unsigned char valueBlock[8][8],
				unsigned char* byte, int plane);

		/**
		 * Finds the complexity of an 8x8 block. The complexity measure used
		 * is the black-white changes along the rows and columns divided by the
		 * total number of posslbe changes. The complexity is returned as a
		 * float.
		 */
		float getComplexity(unsigned char *);

		/**
		 * Returns the complexity threshold.
		 */
		float getComplexityThreshold();

		/**
		 * Returns true if the given 8x8 block is complex and false otherwise.
		 * Uses the complexity measure described in "getComplexity" and compares
		 * it with the threshold set.
		 */
		bool isComplex(unsigned char *);

		/**
		 * Prints out the bit values of an 8x8 block to stdout.
		 */
		void printBitPlane(unsigned char *);

		/**
		 * Prints out the integer values of the color value block (one color of
		 * an 8x8 pixel block).
		 */
		void printValueBlock(unsigned char[8][8]);

		/**
		 * Sets the complexity threshold to use for comparison.
		 */
		void setComplexityThreshold(float);

	private:

        float threshold; // Complexity threshold (alpha value)

        /**
         * Changes the given bit in the given byte to the given value.
         */
		unsigned char changeBitOfByte(unsigned char byte, int bit, int value);

		/**
		 * Returns the value of a bit from a byte (as a 0 or 1).
		 */
		unsigned char getBitOfChar(unsigned char byte, int bit);

};

#endif /* __BlockUtility_H_INCLUDED__ */
