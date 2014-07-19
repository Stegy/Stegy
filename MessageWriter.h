/*
 * MessageWriter.h
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca
 */

#include "StegoCommon.h"

#include <fstream>
#include <string>

using namespace std;

#ifndef MESSAGEWRITER_H_
#define MESSAGEWRITER_H_




class MessageWriter {
public:
	MessageWriter(string fileName);
	void decodeNext(unsigned char* secretBlock);
	int decodeSizeBlock(unsigned char* sizeBlock);
	int setUpMapBlocks(int size);
	void decodeNextMapBlock(unsigned char* secretBlock);
	void decodeNextMessageBlock(unsigned char* secretBlock, int blockIdx,
			int stopIdx);
	bool isConjugated(int blockIndex);

private:
	bool conjBitSet(unsigned char firstByte);

//	const int kBlockSize = 8;
	int numMapBlocks;
	int mapBlockIdx;
	MapBlock* map;
	ofstream output;
};

#endif /* MESSAGEWRITER_H_ */
