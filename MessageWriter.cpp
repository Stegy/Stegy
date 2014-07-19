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

MessageWriter::MessageWriter(string fileName) {
	output.open(fileName, ios::out | ios::binary);
	if (!output.is_open()) {
		cout << "Failed to open file for writing" << endl;
		cout << "error: " << strerror(errno) << endl;
		exit(1);
	}
	mapBlockIdx = -1;
	numMapBlocks = -1;
	map = NULL;
}

//void MessageWriter::decodeNext(unsigned char* secretBlock) {
//	blockIdx++;
//	// If first block read, get the size
//	if (blockIdx == 0) {
//
//	}
//}

int MessageWriter::decodeSizeBlock(unsigned char* sizeBlock) {
	if (conjBitSet(sizeBlock[0])) {
		// TODO Conjugate the block
	}
	// Set conjugation bit to 0
	sizeBlock[0] |= 0x7F;
	int size = 0;
	uint8_t shiftAmt = 56; // 7 bytes
	// Read size bytes into int
	for (int i = 0; i < kBlockSize; i++) {
		size += (sizeBlock[i] << shiftAmt);
		shiftAmt -= 8;
	}
	// TODO - build the map and set up stuff here
	return size;
}

// Returns # of map blocks
int MessageWriter::setUpMapBlocks(int size) {
	bool partBlock = (size % 8 != 0);
	// Base map size in bits
	int mapSize = size / 8 + (partBlock ? 1 : 0);
	// Number of map blocks = number conjugation bits needed for map
	numMapBlocks = ceil((double) mapSize / 63);
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


void MessageWriter::decodeNextMapBlock(unsigned char* secretBlock) {
	mapBlockIdx++;
	// Creates map block from the secret block passed in
	if (conjBitSet(secretBlock[0])) {
		// TODO conjugate the block
	}
	// Add data to the map block
	map[mapBlockIdx].firstRow = secretBlock[0] & 0x80;
	for (int i = 0; i < kBlockSize - 1; i++) {
		map[mapBlockIdx].rows[i] = secretBlock[i + 1];
	}
	// TODO read first part of data if fullTo < 8
	for (int i = map[mapBlockIdx].fullTo; i < kBlockSize - 1; i++) {
		output.write((char*) map[mapBlockIdx].rows[i], 1);
	}
}

void MessageWriter::decodeNextMessageBlock(unsigned char* secretBlock,
		int blockIdx, int stopIdx) {
	if (isConjugated(blockIdx)) {
		// Conjugate secretBlock
	}
	for (int i = 0; i < stopIdx; i++) {
		output.write((char*) secretBlock[i], 1);
	}
}

bool MessageWriter::conjBitSet(unsigned char firstByte) {
	return (firstByte & 0x80) == 1;
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
