/*
 * MessageReader.h
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca
 */

#ifndef MESSAGEREADER_H_
#define MESSAGEREADER_H_

#include "StegoCommon.h"

#include <stdint.h>
#include <string>
#include <vector>

using namespace std;

const unsigned char shiftOutMask[] = {
		0,
		0x01,
		0x03,
		0x07,
		0x0F,
		0x1F,
		0x3F,
		0x7F
};

//// Map block
//struct MapBlock {
//	unsigned char conjBit : 1;
//	unsigned char firstRow : 7;
//	unsigned char rows[kBlockSize - 1] = {0};
//	int fullTo; // Index of the next available row after map data
//};

class MessageReader {
private:
	uint64_t size;
	int shiftAmt;
	unsigned char* fileBuffer;
	int messageIndex;
	int mapIndex;
	MapBlock* map;
	int numMapBlocks;
//	const int kBlockSize = 8;

	void buildMap();

public:
	MessageReader(string fileName);
	unsigned char* getHeader();
	unsigned char* getNext();
	int getNumMapBlocks();
	int getNext(int size, unsigned char* buffer);
	void setMapBit(int blockNumber, bool value);
	MapBlock getNextMapBlock();

};



#endif /* MESSAGEREADER_H_ */
