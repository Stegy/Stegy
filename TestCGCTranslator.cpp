/*
 * TestCGCTranslator.cpp
 *
 *  Created on: Jun 25, 2014
 *      Author: Rebecca
 */

#include <iostream>
#include "CGCTranslator.h"

using namespace std;

const unsigned char BIN1 = 0;
const unsigned char BIN2 = 34;
const unsigned char BIN3 = 105;
const unsigned char BIN4 = 255;
const unsigned char CGC1 = 0;
const unsigned char CGC2 = 51;
const unsigned char CGC3 = 93;
const unsigned char CGC4 = 128;

bool assertEquals(string testName, string msg, unsigned char expected,
		unsigned char actual) {
	if (expected != actual) {
		cout << "FAILED: "
			 << testName
			 << " - " << msg
			 << ": Expected: " << expected
			 << "; Actual: " << actual
			 << endl;
		return false;
	}
	return true;
}

void testCgcToBinary(CGCTranslator tester) {
	unsigned char res = tester.cgcToPbc(CGC1);
	bool passed = true;
	if (!assertEquals("cgcToBinary", "CGC1", BIN1, res)) {
		passed = false;
	}
	res = tester.cgcToPbc(CGC2);
	if (!assertEquals("cgcToBinary", "CGC2", BIN2, res)) {
		passed = false;
	}
	res = tester.cgcToPbc(CGC3);
	if (!assertEquals("cgcToBinary", "CGC3", BIN3, res)) {
		passed = false;
	}
	res = tester.cgcToPbc(CGC4);
	if (!assertEquals("cgcToBinary", "CGC4", BIN4, res)) {
		passed = false;
	}
	if (passed) {
		cout << "PASSED: cgcToBinary" << endl;
	}
}

void testBinaryToCgc(CGCTranslator tester) {
	unsigned char res = tester.pbcToCgc(BIN1);
	bool passed = true;
	if (!assertEquals("binaryToCgc", "BIN1", CGC1, res)) {
		passed = false;
	}
	res = tester.pbcToCgc(BIN2);
	if (!assertEquals("binaryToCgc", "BIN2", CGC2, res)) {
		passed = false;
	}
	res = tester.pbcToCgc(BIN3);
	if (!assertEquals("binaryToCgc", "BIN3", CGC3, res)) {
		passed = false;
	}
	res = tester.pbcToCgc(BIN4);
	if (!assertEquals("binaryToCgc", "BIN4", CGC4, res)) {
		passed = false;
	}
	if (passed) {
		cout << "PASSED: binaryToCgc" << endl;
	}
}

unsigned char toCGC[256];
unsigned char toPBC[256];

// TEMP
// this function builds the canonical gray code array variables
void buildGrayCode()
{
	int i, length, posUp, posDw, cnt;

	length = 1;
	toCGC[0] = 0;
	toPBC[0] = 0;
	cnt = 1;

	while(length < 256)
	{
		posUp = length - 1;
		posDw = length;
		for(i = 0; i < length; i++)
		{
			toCGC[i + posDw] = toCGC[posUp - i] + length;
			toPBC[toCGC[i + posDw]] = cnt++;
		}
		length = length * 2;
	}
	return;
} // buildGrayCode

int main() {
	CGCTranslator tester;
	tester.printCGC();
	tester.printPBC();
	testCgcToBinary(tester);
	testBinaryToCgc(tester);
}
