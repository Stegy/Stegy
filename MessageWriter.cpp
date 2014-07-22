/*
 * MessageWriter.cpp
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca
 */

#include "MessageWriter.h"

#include <iostream>
#include <math.h>

using namespace std;

MessageWriter::MessageWriter(string fileName, BlockUtility* utility) {
//	output.open(fileName, ios::out | ios::binary);
//	if (!output.is_open()) {
//		cout << "Failed to open file for writing" << endl;
//		cout << "error: " << strerror(errno) << endl;
//		exit(1);
//	}
	fpOutput = fopen(fileName.c_str(), "w");
	if (fpOutput == NULL) {
		cout << "Failed to open file for writing" << endl;
		cout << "error: " << strerror(errno) << endl;
		exit(1);
	}

	currentSize = 0;
	mapBlockIdx = -1;
	numMapBlocks = -1;
	map = NULL;
	this->utility = utility;
}

int MessageWriter::decodeSizeBlock(unsigned char* sizeBlock) {
	cout << "orig size block: " << endl;
	utility->printBitPlane(sizeBlock);
	if (conjBitSet(sizeBlock[0])) {
		cout << "conjugated" << endl;
		utility->conjugate(sizeBlock);
		utility->printBitPlane(sizeBlock);
	}
	// Set conjugation bit to 0
	sizeBlock[0] &= 0x7F;
	uint64_t size = 0;
	uint8_t shiftAmt = 56; // 7 bytes
	// Read size bytes into int
	for (int i = 0; i < kBlockSize; i++) {
		bitset<8> x(sizeBlock[i]);
		cout << "size row: " << x << endl;
		size += (sizeBlock[i] << shiftAmt);
		shiftAmt -= 8;
	}
	messageSize = size;
	cout << "In message writer, found size : " << messageSize << endl;
	setUpMapBlocks(messageSize);
	return messageSize;
}

// Returns # of map blocks
int MessageWriter::setUpMapBlocks(int size) {
	cout << "Setting up map blocks" << endl;
	int mapSize = ceil((double) size / 8);
	// Number of map blocks = number conjugation bits needed for map
	numMapBlocks = ceil((double) mapSize / 63);
	cout << "Number of map blocks: " << numMapBlocks << endl;
	// Should actually build this here
	map = new MapBlock[numMapBlocks];
	int i;
	// Initialize map block sizes
	for (i = 0; i < numMapBlocks - 1; i++) {
		map[i].fullTo = kBlockSize;
	}
	map[i].fullTo = ceil((double) (mapSize % 63) / 8);
	return numMapBlocks;
}

int MessageWriter::getNumMapBlocks() {
	return numMapBlocks;
}

int MessageWriter::getMessageSize() {
	return messageSize;
}

void MessageWriter::decodeNextMapBlock(unsigned char* secretBlock) {
	cout << "found map block: " << endl;
	utility->printBitPlane(secretBlock);
	mapBlockIdx++;
	// Creates map block from the secret block passed in
	if (conjBitSet(secretBlock[0])) {
		utility->conjugate(secretBlock);
		cout << "conjugated map block: " << endl;
		utility->printBitPlane(secretBlock);
	}
	// Add data to the map block
	map[mapBlockIdx].firstRow = secretBlock[0] & 0x80;
	for (int i = 0; i < kBlockSize - 1; i++) {
		map[mapBlockIdx].rows[i] = secretBlock[i + 1];
	}
	// TODO read first part of data if fullTo < 8
	for (int i = map[mapBlockIdx].fullTo - 1; i < kBlockSize - 1; i++) {
		cout << "map block write attempt" << endl;
//		output.write((char*) map[mapBlockIdx].rows + i, 1);
		fwrite(map[mapBlockIdx].rows + i, 1, 1, fpOutput);
		currentSize++;
		cout << "After write " << endl;
	}
}

// Returns true when message is done being read
bool MessageWriter::decodeNextMessageBlock(unsigned char* secretBlock,
		int blockIdx) {
	if (isConjugated(blockIdx)) {
		cout << "Found conjugated block " << blockIdx << endl;
		utility->conjugate(secretBlock);
	}
	for (int i = 0; i < kBlockSize && currentSize < messageSize; i++) {
//		output.write((char*) secretBlock + i, 1);
		fwrite(secretBlock + i, 1, 1, fpOutput);
		currentSize++;
	}
	if (currentSize == messageSize) {
		return true;
	}
	return false;
}

bool MessageWriter::conjBitSet(unsigned char firstByte) {
	return ((firstByte & 0x80) >> 7) == 1;
}


bool MessageWriter::isConjugated(int blockIndex) {
	int mapBlock = blockIndex / 63;
	int idxInBlock = blockIndex % 63;
	// value & mask == 0 or == 1
	if (idxInBlock < 7) {
		unsigned char mask = 1 << (kBlockSize - idxInBlock - 2);
		if ((map[mapBlock].firstRow & mask) == 0) { // Set bit in map to 1
			return false;
		}
		return true;
	} else {
		idxInBlock -= 7;
		int rowNumber = idxInBlock / 8;
		int colNumber = idxInBlock % 8;
		unsigned char mask = 1 << (kBlockSize - colNumber - 1);
		if ((map[mapBlock].rows[rowNumber] & mask) == 0) { // Set bit in map to 1
			return false;
		}
		return true;
	}
}

void MessageWriter::closeFile() {
//	output.close();
	fclose(fpOutput);
}
