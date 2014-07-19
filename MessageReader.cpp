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

using namespace std;

MessageReader::MessageReader(string fileName) {
	std::ifstream input(fileName, std::ios::in|std::ios::binary);
	if (!input.is_open()) {
		cout << "failed to open file for reading" << endl;
		cout << "error: " << strerror(errno) << endl;
		exit(1);
	}
	input.seekg(0, ios::end);
	size = input.tellg();
	fileBuffer = new unsigned char[size];
	input.seekg(0, ios::beg);
	input.getline((char*) fileBuffer, size); // TODO Okay for unsigned conversion?
	messageIndex = 0;
	mapIndex = 0;
	buildMap();
}

/**
 * Returns the 8-byte header containing the file size. The most significant bit
 * of the first byte in the array is initially set to 0, and should be used as
 * a conjugation bit.
 */
unsigned char* MessageReader::getHeader() {
	unsigned char* result = new unsigned char[kBlockSize];
	// TODO, check if over max size
	result[0] = (size & 0x7F00000000000000) >> 56; // 7 bytes
	uint64_t mask = 0x00FF000000000000;
	uint8_t shiftAmt = 48; // 6 bytes
	for (int i = 1; i < kBlockSize; i++) {
		result[i] = (size & mask) >> shiftAmt;
		mask >>= 8;
		shiftAmt -= 8;
	}
	return result;
}

/**
 * Builds the conjugation map. This includes 1 bit for each block of message
 * data. If there is space remaining in the last map block, the beginning of
 * the message data is stored inside.
 */
void MessageReader::buildMap() {
	// Finds if the message will partially fill its last block (extra bytes)
	bool partBlock = (size % 8 != 0);
	// Base map size in bits
	int mapSize = size / 8 + (partBlock ? 1 : 0);
	// Number of map blocks = number conjugation bits needed for map
	numMapBlocks = ceil((double) mapSize / 63);
	mapSize += numMapBlocks; // Total map size in bits
	map = new MapBlock[numMapBlocks];
	int i;
	// Initialize map block sizes
	for (i = 0; i < numMapBlocks - 1; i++) {
		map[i].fullTo = kBlockSize;
	}
	map[i].fullTo = ceil((double) (mapSize % 63) / 8);
	int readSize = kBlockSize - map[i].fullTo;
	if (readSize > 0) {
		cout << "Reading" << endl;
//		int offset = map[i].fullTo;
		unsigned char tempBuf[readSize];
		int res = getNext(readSize, tempBuf);
		if (res != readSize && res < size) {
			// TODO handle
			cout << "empty read" << endl;
		} else {
			int k = 0;
			for (int j = map[i].fullTo - 1; j < kBlockSize - 1; j++) {
				map[i].rows[j] = tempBuf[k++];
			}
			cout << "read : " << res << endl;
		}
	} else {
		cout << "read size < 0" << endl;
	}
	cout << "Map blocks at end of build map: " << endl;
	for (int j = 0; j < numMapBlocks; j++) {
		cout << "full to: " << map[j].fullTo << endl;
	}
}

/**
 * Returns the number of blocks needed for the conjugation map. These should
 * be reserved, and the map should be embedded at the end.
 */
int MessageReader::getNumMapBlocks() {
	return numMapBlocks;
}

/**
 * Gets the next readSize bytes from the message, if possible, and adds them
 * to the passed buffer. Returns the number of bytes read into the buffer.
 */
int MessageReader::getNext(int readSize, unsigned char* readBuffer) {
	int i;
	cout << "in reader, readsize = " << readSize << endl;
	for (i = 0; i < readSize && messageIndex < size; i++) {
		readBuffer[i] = fileBuffer[messageIndex++];
	}
	return i;
}

/**
 * Returns the next map block available. Should be called when all of the
 * secret message has been embedded and the corresponding map bits have been
 * set.
 */
MapBlock MessageReader::getNextMapBlock() {
	// TODO: Conjugate and set conjugation bit!
	return map[mapIndex++];
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

/**
 * Main process:
 * - get header, conjugate if necessary
 * - get number of map blocks, reserve this space
 * - until message is over:
 * 		get next message block
 * 		conjugate if necessary
 * 		set conjugation map
 * 		embed message
 * - for each map block:
 * 		get the map block
 * 		conjugate if necessary
 * 		set its conjugation bit
 * 		embed in reserved place
 */

