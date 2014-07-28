/*
 * cgc_translation.h
 *
 * Contains the CGCTranslator class definition.
 *
 *  Created on: Jun 25, 2014
 *      Author: Rebecca Strautman
 */

#ifndef CGC_TRANSLATION_H_
#define CGC_TRANSLATION_H_

const int NUM_CODES = 256;
const int CODE_SIZE = 8;

/**
 * This class constructs a Canonical Gray Code (CGC) and arrays to convert
 * bytes between CGC and Pure Binary Code (PBC).
 */
class CGCTranslator {

public:
	/**
	 * Constructor constructs the translation arrays for 8-bit CGC and PBC.
	 */
	CGCTranslator();

	/**
	 * Takes a byte in CGC and returns the PBC translation.
	 */
	unsigned char cgcToPbc(unsigned char byteIn);

	/**
	 * Takes a byte in BPC and returns the CGC translation.
	 */
	unsigned char pbcToCgc(unsigned char byteIn);

	/**
	 * Prints the CGC generated to stdout (for testing).
	 */
	void printCGC();

	/**
	 * Prints the BPC-indexed Gray code to stdout (for testing).
	 */
	void printPBC();

private:

	unsigned char binIndexedCgc[NUM_CODES];
	unsigned char cgcIndexedBin[NUM_CODES];

	/**
	 * Recursively builds the CGC of the given number of bits.
	 */
	void genNBitGrayCode(int n);
};

#endif /* CGC_TRANSLATION_H_ */
