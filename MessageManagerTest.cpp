#include "BlockUtility.h"
#include "MessageReader.h"
#include "MessageWriter.h"
#include "CGCTranslator.h"

#include <iostream>
#include <unistd.h>
#include <fstream>
#include <math.h>

int main() {
	std::cout << "Beginning" << std::endl;
	BlockUtility utility;
	utility.setComplexityThreshold(0.2);
	MessageReader* mr = new MessageReader("bin/gettysburg.txt", &utility);
	cout << "size: " << mr->getSize() << endl;
	unsigned char header[8];
	mr->getSizeBlock(header);
	cout << "Size bits found:" << endl;
	for (int i = 0; i < kBlockSize; i++) {
		bitset<8> x(header[i]);
		cout << " " << x << " ";
	}
	uint64_t testFileSize;
	int mapSize = mr->getNumMapBlocks();
	cout << "Found map size: " << mapSize << endl;
	mr->setMapBit(0, true);
	mr->setMapBit(2, true);
	mr->setMapBit(7, true);
	mr->setMapBit(13, true);
	mr->setMapBit(42, true);
	mr->setMapBit(62, true);
	mr->setMapBit(63, true);
	mr->setMapBit(126, true);

	unsigned char mapBlock[8];
//	MapBlock mapBlock = mr->getNextMapBlock();
//	MapBlock mapBlock2 = mr->getNextMapBlock();
//	MapBlock mapBlock3 = mr->getNextMapBlock();
	// Map block 1:
	mr->getNextMapBlock(mapBlock);
	cout << "Map block found 1: " << endl;
//	cout << "conj bit: " << mapBlock.conjBit << endl;
//	cout << "map full to: " << mapBlock.fullTo << endl;
//	bitset<kBlockSize - 1> x(mapBlock.firstRow);
//	cout << "first row: " << x << endl;
//	for (int i = 0; i < kBlockSize - 1; i++) {
//		bitset<kBlockSize> x(mapBlock.rows[i]);
//		cout << x << endl;
//	}
	utility.printBitPlane(mapBlock);
	// Map block 2:
	mr->getNextMapBlock(mapBlock);
	cout << "Map block found 2: " << endl;
//	cout << "conj bit: " << mapBlock2.conjBit << endl;
//	cout << "map full to: " << mapBlock2.fullTo << endl;
//	bitset<kBlockSize - 1> y(mapBlock2.firstRow);
//	cout << "first row: " << y << endl;
//	for (int i = 0; i < kBlockSize - 1; i++) {
//		bitset<kBlockSize> y(mapBlock2.rows[i]);
//		cout << y << endl;
//	}
	utility.printBitPlane(mapBlock);
	// Map block 3:
	cout << "Map block found 3: " << endl;
//	cout << "conj bit: " << mapBlock3.conjBit << endl;
//	cout << "map full to: " << mapBlock3.fullTo << endl;
//	bitset<kBlockSize - 1> z(mapBlock3.firstRow);
//	cout << "first row: " << z << endl;
//	for (int i = 0; i < kBlockSize - 1; i++) {
//		bitset<kBlockSize> z(mapBlock3.rows[i]);
//		cout << z << endl;
//	}
	mr->getNextMapBlock(mapBlock);
	utility.printBitPlane(mapBlock);
	// Print out last 3 rows
	cout << "last 3 rows: " << endl;
	for (int i = 4; i < 8; i++) {
		cout << mapBlock[i] << endl;
//		cout << mapBlock3.rows[i] << endl;
	}
//
//	// Open new file for writing message
	unsigned char testBlock[8];
	unsigned char testBlockCGC[8];
//	MessageWriter* writer = new MessageWriter("testOutput.txt", &utility);
//	cout << "Past writer" << endl;

	// Writing test
	MessageReader testRead("bin/input1.bmp", &utility);
	MessageWriter testWrite("testOutput.bmp", &utility);
	CGCTranslator translator;
	testRead.getSizeBlock(testBlock);
	testWrite.decodeSizeBlock(testBlock);
	for (int i = 0; i < testRead.getNumMapBlocks(); i++ ){
		testRead.getNextMapBlock(testBlock);
		testWrite.decodeNextMapBlock(testBlock);
	}

	for (int i = 0; i < ceil((double) testRead.getSize() / 8); i++) {
		testRead.getNext(testBlock);
		cout << "TEST BLOCK: " << testBlock[0] << endl;
		for (int i = 0; i < 8; i++ ) {
			cout << "next byte: " << testBlock[i] << endl;
		}
				for (int j = 0; j < 8; j++ ) {
					testBlockCGC[j] = translator.cgcToPbc(testBlock[j]);
				}
				for (int j = 0; j < 8; j++) {
					testBlock[j] = translator.pbcToCgc(testBlockCGC[j]);
				}
		testWrite.decodeNextMessageBlock(testBlock, 0);
	}
	cout << "Finished read/write test to testOutput.txt" << endl;
}
