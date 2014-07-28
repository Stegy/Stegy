/*
 * MessageWriter.cpp
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca
 */

#include "MessageWriter.h"

#include <errno.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

MessageWriter::MessageWriter(string fileName, BlockUtility* utility) {
	// Open file for writing
	fpOutput = fopen(fileName.c_str(), "w");
	if (fpOutput == NULL) {
		cout << "Failed to open file for writing" << endl;
		cout << "error: " << strerror(errno) << endl;
		exit(1);
	}
	// Initialize variables
	currentSize = 0;
	messageSize = -1;
	mapBlockIdx = -1;
	numMapBlocks = -1;
	map = NULL;
	this->utility = utility;
}

MessageWriter::~MessageWriter() {
	if (map != NULL) {
		delete[] map;
	}
}

void MessageWriter::closeFile() {
	fclose(fpOutput);
}

void MessageWriter::decodeNextMapBlock(unsigned char* secretBlock) {
	mapBlockIdx++;
	// Conjugate if necessary
	if (conjBitSet(secretBlock[0])) {
		utility->conjugate(secretBlock);
	}
	// Add the data to the in-memory conjugation map
	map[mapBlockIdx].firstRow = secretBlock[0] & 0x7F;
	for (int i = 0; i < kBlockSize - 1; i++) {
		map[mapBlockIdx].rows[i] = secretBlock[i + 1];
	}
	// If the map block contains bytes from the secret message at the end,
	// write these to the file
	for (int i = map[mapBlockIdx].fullTo - 1; i < kBlockSize - 1; i++) {
		fwrite(map[mapBlockIdx].rows + i, 1, 1, fpOutput);
		currentSize++;
	}
}

bool MessageWriter::decodeNextMessageBlock(unsigned char* secretBlock,
		int blockIdx) {
	// Conjugate if necessary
	if (isConjugated(blockIdx)) {
		utility->conjugate(secretBlock);
	}
	for (int i = 0; i < kBlockSize && currentSize < messageSize; i++) {
		fwrite(secretBlock + i, 1, 1, fpOutput);
		currentSize++;
	}
	return (currentSize == messageSize);
}

int MessageWriter::decodeSizeBlock(unsigned char* sizeBlock) {
	// Conjugate if necessary
	if (conjBitSet(sizeBlock[0])) {
		utility->conjugate(sizeBlock);
	}
	// Set conjugation bit to 0
	sizeBlock[0] &= 0x7F;
	uint64_t size = 0;
	uint8_t shiftAmt = 56; // 7 bytes
	// Read size bytes into an int
	for (int i = 0; i < kBlockSize; i++) {
		size += (sizeBlock[i] << shiftAmt);
		shiftAmt -= 8;
	}
	// Check for bad size
	if (messageSize < 0) {
		cout << "Invalid stego image - bad message size read: "
				<< messageSize << endl;
		exit(1);
	}
	// Set variables and construct map
	messageSize = size;
	setUpMapBlocks(messageSize);
	return messageSize;
}

int MessageWriter::getCurrentSize() {
	return currentSize;
}

int MessageWriter::getMessageSize() {
	return messageSize;
}

int MessageWriter::getNumMapBlocks() {
	return numMapBlocks;
}


bool MessageWriter::conjBitSet(unsigned char firstByte) {
	return ((firstByte & 0x80) >> 7) == 1;
}

bool MessageWriter::isConjugated(int blockIndex) {
	int mapBlock = blockIndex / 63; // Find map block
	int idxInBlock = blockIndex % 63; // Find index within map block
	if (idxInBlock < 7) { // Bit is in first row
		unsigned char mask = 1 << (kBlockSize - idxInBlock - 2);
		if ((map[mapBlock].firstRow & mask) == 0) { // Check if bit == 0
			return false;
		}
		return true;
	} else { // Bit is in remaining rows
		idxInBlock -= 7;
		int rowNumber = idxInBlock / 8;
		int colNumber = idxInBlock % 8;
		unsigned char mask = 1 << (kBlockSize - colNumber - 1);
		if ((map[mapBlock].rows[rowNumber] & mask) == 0) { // Check if bit == 0
			return false;
		}
		return true;
	}
}

int MessageWriter::setUpMapBlocks(int size) {
	int mapSize = ceil((double) size / 8);
	// Number of map blocks = number conjugation bits needed for map
	numMapBlocks = ceil((double) mapSize / 63);
	map = new MapBlock[numMapBlocks];
	int i;
	// Initialize map blocks
	for (i = 0; i < numMapBlocks - 1; i++) {
		map[i].fullTo = kBlockSize;
		map[i].firstRow = 0;
		for (int j = 0; j < kBlockSize - 1; j++) {
			map[i].rows[j] = 0;
		}
	}
	map[i].firstRow = 0;
	for (int j = 0; j < kBlockSize - 1; j++) {
		map[i].rows[j] = 0;
	}
	// Determine how much of last block is used for map
	map[i].fullTo = ceil((double) (mapSize % 63) / 8);
	return numMapBlocks;
}
