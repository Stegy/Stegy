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
	unsigned char res = tester.cgcToBinary(CGC1);
	bool passed = true;
	if (!assertEquals("cgcToBinary", "CGC1", BIN1, res)) {
		passed = false;
	}
	res = tester.cgcToBinary(CGC2);
	if (!assertEquals("cgcToBinary", "CGC2", BIN2, res)) {
		passed = false;
	}
	res = tester.cgcToBinary(CGC3);
	if (!assertEquals("cgcToBinary", "CGC3", BIN3, res)) {
		passed = false;
	}
	res = tester.cgcToBinary(CGC4);
	if (!assertEquals("cgcToBinary", "CGC4", BIN4, res)) {
		passed = false;
	}
	if (passed) {
		cout << "PASSED: cgcToBinary" << endl;
	}
}

void testBinaryToCgc(CGCTranslator tester) {
	unsigned char res = tester.binaryToCgc(BIN1);
	bool passed = true;
	if (!assertEquals("binaryToCgc", "BIN1", CGC1, res)) {
		passed = false;
	}
	res = tester.binaryToCgc(BIN2);
	if (!assertEquals("binaryToCgc", "BIN2", CGC2, res)) {
		passed = false;
	}
	res = tester.binaryToCgc(BIN3);
	if (!assertEquals("binaryToCgc", "BIN3", CGC3, res)) {
		passed = false;
	}
	res = tester.binaryToCgc(BIN4);
	if (!assertEquals("binaryToCgc", "BIN4", CGC4, res)) {
		passed = false;
	}
	if (passed) {
		cout << "PASSED: binaryToCgc" << endl;
	}
}

int main() {
	CGCTranslator tester;
	tester.printGrayCode();
	tester.printBinaryCode();
	testCgcToBinary(tester);
	testBinaryToCgc(tester);
}
