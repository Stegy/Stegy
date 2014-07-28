/*
 * MessageReader.cpp
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca Strautman
 */

#include "MessageReader.h"

#include <errno.h>
#include <fstream>
#include <math.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

MessageReader::MessageReader(string fileName, BlockUtility* utility) {
	// Open message file
	fpInput = fopen(fileName.c_str(), "r");
	if (fpInput == NULL) {
		cout << "failed to open file for reading" << endl;
		cout << "error: " << strerror(errno) << endl;
		exit(1);
	}
	// Find size
	fseek(fpInput, 0 ,SEEK_END);
	size = ftell(fpInput);
	// Read message into memory
	int pos = 0;
	fseek(fpInput, pos, SEEK_SET);
	fileBuffer = new unsigned char[size];
	fread(fileBuffer, 1, size, fpInput);
	fclose(fpInput);
	// Initialize variables
	messageIndex = 0;
	mapIndex = 0;
	this->utility = utility;
	buildMap();
}

MessageReader::~MessageReader() {
	delete[] fileBuffer;
	delete[] map;
}

int MessageReader::getCurrentSize() {
	return messageIndex;
}

bool MessageReader::getNext(unsigned char* readBuffer) {
	for (int i = 0; i < kBlockSize && messageIndex < size; i++) {
		readBuffer[i] = fileBuffer[messageIndex++];
	}
	return (messageIndex == size);
}

bool MessageReader::getNextMapBlock(unsigned char* block) {
	if (mapIndex < numMapBlocks) {
		block[0] = map[mapIndex].firstRow;
		block[0] &= 0x7F; // Initialize conjugation bit to 0
		// Copy the map block into the buffer
		for (int i = 1; i < kBlockSize; i++) {
			block[i] = map[mapIndex].rows[i-1];
		}
		// Conjugate if necessary
		if (!utility->isComplex(block)) {
			utility->conjugate(block);
			// Set conjugation bit to 1
			block[0] |= (1 << (kBlockSize - 1));
		}
		mapIndex++;
	}
	return (mapIndex == numMapBlocks);
}

int MessageReader::getNumMapBlocks() {
	return numMapBlocks;
}

int MessageReader::getSize() {
	return size;
}

void MessageReader::getSizeBlock(unsigned char* result) {
	// In first row of result block, set conjugation bit to 0, and
	// keep remaining 7 bits
	result[0] = (size >> 56) & 0x7F; // Shifted 7 bytes out
	uint64_t mask = 0x00FF000000000000ULL;
	uint8_t shiftAmt = 48; // 6 bytes
	for (int i = 1; i < kBlockSize; i++) {
		// Mask for the desired byte of "size" and shift it to the right
		result[i] = (size & mask) >> shiftAmt;
		mask >>= 8;
		shiftAmt -= 8;
	}
	// Conjugate if necessary
	if (!utility->isComplex(result)) {
		utility->conjugate(result);
		// Set conjugation bit to 1
		result[0] |= 0x80;
	}
}

void MessageReader::setMapBit(int blockIndex, bool conjugated) {
	int mapBlock = blockIndex / 63; // Map block index
	int idxInBlock = blockIndex % 63; // Index of bit within the map block
	if (idxInBlock < 7) { // Bit is in first row
		unsigned char mask = 1 << (kBlockSize - idxInBlock - 2);
		if (conjugated) {
			// Set bit in map to 1
			map[mapBlock].firstRow |= mask;
		}
	} else { // Bit is in remaining rows
		idxInBlock -= 7;
		int rowNumber = idxInBlock / 8;
		int colNumber = idxInBlock % 8;
		unsigned char mask = 1 << (kBlockSize - colNumber - 1);
		if (conjugated) {
			// Set bit in map to 1
			map[mapBlock].rows[rowNumber] |= mask;
		}
	}
}

void MessageReader::buildMap() {
	// Map size in bits (number of conjugation bits needed for message)
	int mapSize = ceil((double) size / 8);
	// Number of map blocks needed
	numMapBlocks = ceil((double) mapSize / 63);
	map = new MapBlock[numMapBlocks];
	int i;
	// Initialize map block sizes
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
	// Fill in remainder of last map block with message data
	int readSize = kBlockSize - map[i].fullTo;
	if (readSize > 0) {
		int readIdx;
		for (readIdx = 0; readIdx < readSize && messageIndex < size;
				readIdx++) {
			map[i].rows[map[i].fullTo - 1 + readIdx]
			            = fileBuffer[messageIndex++];
		}
		if (readIdx != readSize && readIdx < size) {
			cout << "Error reading from message file" << endl;
			exit(1);
		}
	}
}
