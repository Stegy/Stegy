#include "MessageReader.h"
#include "MessageWriter.h"

#include <iostream>
#include <unistd.h>
#include <fstream>

int main() {
	std::cout << "Beginning" << std::endl;
	MessageReader* mr = new MessageReader("bin/gettysburg.txt");
	unsigned char* header = mr->getHeader();
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

	MapBlock mapBlock = mr->getNextMapBlock();
	MapBlock mapBlock2 = mr->getNextMapBlock();
	MapBlock mapBlock3 = mr->getNextMapBlock();
	// Map block 1:
	cout << "Map block found 1: " << endl;
	cout << "conj bit: " << mapBlock.conjBit << endl;
	cout << "map full to: " << mapBlock.fullTo << endl;
	bitset<kBlockSize - 1> x(mapBlock.firstRow);
	cout << "first row: " << x << endl;
	for (int i = 0; i < kBlockSize - 1; i++) {
		bitset<kBlockSize> x(mapBlock.rows[i]);
		cout << x << endl;
	}
	// Map block 2:
	cout << "Map block found 2: " << endl;
	cout << "conj bit: " << mapBlock2.conjBit << endl;
	cout << "map full to: " << mapBlock2.fullTo << endl;
	bitset<kBlockSize - 1> y(mapBlock2.firstRow);
	cout << "first row: " << y << endl;
	for (int i = 0; i < kBlockSize - 1; i++) {
		bitset<kBlockSize> y(mapBlock2.rows[i]);
		cout << y << endl;
	}
	// Map block 3:
	cout << "Map block found 3: " << endl;
	cout << "conj bit: " << mapBlock3.conjBit << endl;
	cout << "map full to: " << mapBlock3.fullTo << endl;
	bitset<kBlockSize - 1> z(mapBlock3.firstRow);
	cout << "first row: " << z << endl;
	for (int i = 0; i < kBlockSize - 1; i++) {
		bitset<kBlockSize> z(mapBlock3.rows[i]);
		cout << z << endl;
	}
	// Print out last 3 rows
	cout << "last 3 rows: " << endl;
	for (int i = 4; i < 7; i++) {
		cout << mapBlock3.rows[i] << endl;
	}

	// Open new file for writing message
	MessageWriter* writer = new MessageWriter("testOutput.txt");
	cout << "Past writer" << endl;



}
