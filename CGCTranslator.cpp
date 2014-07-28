/*
 * cgc_translation.cpp
 *
 *  Created on: Jun 25, 2014
 *      Author: Rebecca Strautman
 */


#include "CGCTranslator.h"

#include <bitset>
#include <iostream>
#include <math.h>

using namespace std;

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

unsigned char CGCTranslator::cgcToPbc(unsigned char byteIn) {
	return cgcIndexedBin[byteIn];
}

unsigned char CGCTranslator::pbcToCgc(unsigned char byteIn) {
	return binIndexedCgc[byteIn];
}

void CGCTranslator::printCGC() {
	cout << "Gray code generated: " << endl;
	for (int i = 0; i < NUM_CODES; i++ ) {
		std::bitset<8> x(binIndexedCgc[i]);
		cout << i << ": " << x << endl;
	}
}

void CGCTranslator::printPBC() {
	cout << "Binary code (indexed by CGC):" << endl;
	for (int i = 0; i < NUM_CODES; i++ ) {
		std::bitset<8> x(cgcIndexedBin[i]);
		cout << i << ": " << x << endl;
	}
}

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
