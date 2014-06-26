/*
 * cgc_translation.cpp
 *
 *  Created on: Jun 25, 2014
 *      Author: Rebecca
 */


#include "CGCTranslator.h"

#include <iostream>
#include <math.h>

using namespace std;

/**
 * Constructor initializes the arrays of CGC and binary used for translation.
 */
CGCTranslator::CGCTranslator() {
	// Initialize arrays to 0s
	for (int i = 0; i < NUM_CODES; i++) {
		binIndexedCgc[i] = 0;
		cgcIndexedBin[i] = 0;
	}
	genNBitGrayCode(CODE_SIZE);
	// Create reverse index
	for (int i = 0; i < NUM_CODES; i++) {
		cgcIndexedBin[binIndexedCgc[i]] = i;
	}
}

/**
 * Helper method recursively builds the CGC of the given number of bits.
 */
void CGCTranslator::genNBitGrayCode(int n) {
	if (n == 0) {
		return;
	}
	genNBitGrayCode(n-1);
	// "Mirror" code of n-1 bits
	int prevSize = pow(2, n-1);
	int i = prevSize - 1;
	int j = prevSize;
	for (  ; i >= 0; i--, j++) {
		binIndexedCgc[j] = binIndexedCgc[i];
	}
	// Add "1" bit to LHS of mirrored n-1 code
	for (j = 0; j < prevSize; j++) {
		binIndexedCgc[j + prevSize] = binIndexedCgc[j + prevSize] | prevSize;
	}
}

/**
 * Prints the Gray code generated to stdout.
 */
void CGCTranslator::printGrayCode() {
	cout << "Gray code generated: " << endl;
	for (int i = 0; i < NUM_CODES; i++ ) {
		std::bitset<8> x(binIndexedCgc[i]);
		cout << i << ": " << x << endl;
	}
}

/**
 * Prints binary code indexed by CGC to stdout.
 */
void CGCTranslator::printBinaryCode() {
	cout << "Binary code (indexed by CGC):" << endl;
	for (int i = 0; i < NUM_CODES; i++ ) {
		std::bitset<8> x(cgcIndexedBin[i]);
		cout << i << ": " << x << endl;
	}
}

/**
 * Translates the given byte from CGC to binary.
 */
unsigned char CGCTranslator::cgcToBinary(unsigned char byteIn) {
	return cgcIndexedBin[byteIn];
}

/**
 * Translates the given byte from binary to CGC.
 */
unsigned char CGCTranslator::binaryToCgc(unsigned char byteIn) {
	return binIndexedCgc[byteIn];
}
