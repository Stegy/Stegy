/*
 * MessageReader.cpp
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca
 */

#include "MessageReader.h"

#include <fstream>
#include <math.h>
#include <iostream>
#include <stdio.h>

using namespace std;

MessageReader::MessageReader(string fileName, BlockUtility* utility) {
//	std::ifstream input(fileName, std::ios::in|std::ios::binary);
//	if (!input.is_open()) {
//		cout << "failed to open file for reading" << endl;
//		cout << "error: " << strerror(errno) << endl;
//		exit(1);
//	}
	fpInput = fopen(fileName.c_str(), "r");
	if (fpInput == NULL) {
		cout << "failed to open file for reading" << endl;
		cout << "error: " << strerror(errno) << endl;
		exit(1);
	}
	fseek(fpInput, 0 ,SEEK_END);
	size = ftell(fpInput);
//	input.seekg(0, ios::end);
//	size = input.tellg();
	int pos;
	fseek(fpInput, pos, SEEK_SET);
	cout << "seek to beginning" << endl;
	fileBuffer = new unsigned char[size];
	cout << "made file buffer of size " << size << endl;

	unsigned char test = fileBuffer[0];
	cout << " Test buffer before read: " << test << endl;


//	input.seekg(0, ios::beg);
//	input.getline((char*) fileBuffer, size); // TODO Okay for unsigned conversion?
	fread(fileBuffer, 1, size, fpInput);
	fclose(fpInput);
//	cout << "After read" << endl;
//	for (int i = 0; i < size; i++) {
//		cout << fileBuffer[i] << endl;
//	}
	messageIndex = 0;
	mapIndex = 0;
	this->utility = utility;
	buildMap();
}

/**
 * Returns the 8-byte header containing the file size. The most significant bit
 * of the first byte in the array is initially set to 0, and should be used as
 * a conjugation bit. Assumes the size fits in 63 bits.
 */
void MessageReader::getSizeBlock(unsigned char* result) {
	// In first row of result block, set conjugation bit to 0, and
	// keep remaining 7 bits
	cout << "IN SIZE BLOCK: " << fileBuffer[0] << endl;
	result[0] = (size >> 56) & 0x7F; // Shifted 7 bytes out
	uint64_t mask = 0x00FF000000000000;
	uint8_t shiftAmt = 48; // 6 bytes
	for (int i = 1; i < kBlockSize; i++) {
		// Mask for desired byte of size and shift it to the right
		result[i] = (size & mask) >> shiftAmt;
		mask >>= 8;
		shiftAmt -= 8;
	}
	if (!utility->isComplex(result)) {
		utility->conjugate(result);
		// Set conjugation bit to 1
		result[0] |= 0x80;
	}
}

/**
 * Builds the conjugation map. This includes 1 bit for each block of message
 * data. If there is space remaining in the last map block, the beginning of
 * the message data is stored inside.
 */
void MessageReader::buildMap() {
	cout << "IN BUILD MAP:" << fileBuffer[0] << endl;
	// Map size in bits (number of conjugation bits needed for message)
	int mapSize = ceil((double) size / 8);
	// Number of map blocks needed
	numMapBlocks = ceil((double) mapSize / 63);
	map = new MapBlock[numMapBlocks];
	int i;
	// Initialize map block sizes
	for (i = 0; i < numMapBlocks - 1; i++) {
		map[i].fullTo = kBlockSize;
	}
	// Find how much of the last map block is taken up
	map[i].fullTo = ceil((double) (mapSize % 63) / 8);
	// Fill in remainder of last map block with message data
	int readSize = kBlockSize - map[i].fullTo;
	cout << "Before map read" << endl;
	if (readSize > 0) {
		int readIdx;
		for (readIdx = 0; readIdx < readSize && messageIndex < size;
				readIdx++) {
			cout << "Read message index: " << messageIndex << endl;
			cout << "map index: " << map[i].fullTo - 1 + readIdx << endl;
			map[i].rows[map[i].fullTo - 1 + readIdx]
			            = fileBuffer[messageIndex++];
		}
		if (readIdx != readSize && readIdx < size) {
			cout << "Error reading from message file" << endl;
			exit(1);
		}
	}
	cout << "After map read";
}

/**
 * Returns the number of blocks needed for the conjugation map. These should
 * be reserved, and the map should be embedded at the end.
 */
int MessageReader::getNumMapBlocks() {
	return numMapBlocks;
}

int MessageReader::getSize() {
	return size;
}

/**
 * Gets the next readSize bytes from the message, if possible, and adds them
 * to the passed buffer. Returns the number of bytes read into the buffer.
 */
bool MessageReader::getNext(int readSize, unsigned char* readBuffer) {
	cout << "IN GET NEXT:" << fileBuffer[0] << endl;
	for (int i = 0; i < readSize && messageIndex < size; i++) {
		cout << "wrote byte: " << fileBuffer[messageIndex] << endl;
		readBuffer[i] = fileBuffer[messageIndex++];
	}
	return (messageIndex == size);
}

/**
 * Returns the next map block available. Should be called when all of the
 * secret message has been embedded and the corresponding map bits have been
 * set.
 */
bool MessageReader::getNextMapBlock(unsigned char* block) {
	block[0] = map[mapIndex].firstRow;
	block[0] &= 0x7F; // Initialize conjugation bit to 0
	for (int i = 1; i < kBlockSize; i++) {
		block[i] = map[mapIndex].rows[i-1];
	}
	cout << "Constructed map block: " << endl;
	utility->printBitPlane(block);
	bool test = utility->isComplex(block);
	cout << "Found complexity: " << test << endl;
	if (!utility->isComplex(block)) {
		utility->conjugate(block);
		cout << "conjugated map block" << endl;
		block[0] |= (1 << (kBlockSize - 1));
	}
	mapIndex++;
	return (mapIndex == numMapBlocks);
}

/**
 * Sets the bit in the conjugation map for the given block. The blockIndex
 * passed in is the index of the secret block being embedded (starting with 0),
 * and the parameter "conjugated" should be true if the block was conjugated
 * and false otherwise. It sets the corresponding bit to 1 if conjugated is
 * true, and 0 if conjugated is false.
 */
void MessageReader::setMapBit(int blockIndex, bool conjugated) {
	int mapBlock = blockIndex / 63;
	int idxInBlock = blockIndex % 63;
	if (idxInBlock < 7) {
		unsigned char mask = 1 << (kBlockSize - idxInBlock - 2);
		if (conjugated) { // Set bit in map to 1
			map[mapBlock].firstRow |= mask;
		}
	} else {
		idxInBlock -= 7;
		int rowNumber = idxInBlock / 8;
		int colNumber = idxInBlock % 8;
		unsigned char mask = 1 << (kBlockSize - colNumber - 1);
		if (conjugated) { // Set bit in map to 1
			map[mapBlock].rows[rowNumber] |= mask;
		}
	}
}
