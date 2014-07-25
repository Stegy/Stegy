/*
 * MessageWriter.h
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca
 */

#include "BlockUtility.h"
#include "StegoCommon.h"

#include <fstream>
#include <stdio.h>
#include <string>

using namespace std;

#ifndef MESSAGEWRITER_H_
#define MESSAGEWRITER_H_




class MessageWriter {
public:
	MessageWriter(string fileName, BlockUtility* utility);
	void decodeNext(unsigned char* secretBlock);
	int decodeSizeBlock(unsigned char* sizeBlock);
	int setUpMapBlocks(int size);
	void decodeNextMapBlock(unsigned char* secretBlock);
	bool decodeNextMessageBlock(unsigned char* secretBlock, int blockIdx);
	bool isConjugated(int blockIndex);
	int getNumMapBlocks();
	int getMessageSize();
	int getCurrentSize();
	void closeFile();

private:
	bool conjBitSet(unsigned char firstByte);

//	const int kBlockSize = 8;
	int numMapBlocks;
	int mapBlockIdx;
	MapBlock* map;
	ofstream output;
	int messageSize;
	int currentSize;
	BlockUtility* utility;
	FILE* fpOutput;
};

#endif /* MESSAGEWRITER_H_ */
