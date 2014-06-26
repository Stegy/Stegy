/*
 * cgc_translation.h
 *
 *  Created on: Jun 25, 2014
 *      Author: Rebecca
 */

#ifndef CGC_TRANSLATION_H_
#define CGC_TRANSLATION_H_

const int NUM_CODES = 256;
const int CODE_SIZE = 8;

class CGCTranslator {
private:
	unsigned char binIndexedCgc[NUM_CODES];
	unsigned char cgcIndexedBin[NUM_CODES];
	void genNBitGrayCode(int n);
public:
	CGCTranslator();
	unsigned char cgcToBinary(unsigned char byteIn);
	unsigned char binaryToCgc(unsigned char byteIn);
	void printGrayCode();
	void printBinaryCode();
};

#endif /* CGC_TRANSLATION_H_ */
