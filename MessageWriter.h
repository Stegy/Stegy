/*
 * MessageWriter.h
 *
 * Contains the MessageWriter class definition.
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca Strautman
 */

#include "BlockUtility.h"
#include "StegoCommon.h"

#include <fstream>
#include <stdio.h>
#include <string>

using namespace std;

#ifndef MESSAGEWRITER_H_
#define MESSAGEWRITER_H_

/**
 * Used to write a secret message to a file as it is decoded from a stego
 * image using BPCS steganography. It reads a size block containing the size of
 * the message to be read, and constructs a conjugation map of the correct size
 * accordingly. It then reads the map blocks as they are passed in, populating
 * the conjugation map. It then reads blocks of the secret message as they are
 * passed in, conjugating if necessary according to the map, and writing them
 * to the file.
 */
class MessageWriter {
public:
	/**
	 * Constructor opens the given file for writing and initializes variables.
	 * It requires a BlockUtility object with the complexity threshold set.
	 */
	MessageWriter(string fileName, BlockUtility* utility);

	/**
	 * Frees memory for the conjugation map.
	 */
	~MessageWriter();

	/**
	 * Closes the file, completing the write of the message.
	 */
	void closeFile();

	/**
	 * Takes a map block extracted from a stego image. Should be called after
	 * the size block has been decoded. Conjugates the block if necessary, and
	 * uses the information to populate the conjugation map. If the map block
	 * also contains message data, it writes this to the output file.
	 */
	void decodeNextMapBlock(unsigned char* secretBlock);

	/**
	 * Takes a secret message block extracted from a stego image. Should be
	 * called after the size and all map blocks have been decoded. Conjugates
	 * the block if necessary and writes the data to the output file. Returns
	 * true when the entire message has been written to the file (according to
	 * the size found).
	 */
	bool decodeNextMessageBlock(unsigned char* secretBlock, int blockIdx);

	/**
	 * Takes a secret block extracted from a stego image containing the message
	 * size - this should be the first complex block found. Conjugates the block
	 * if necessary and reads the message size. According to this size, creates
	 * the conjugation map. Returns the message size.
	 */
	int decodeSizeBlock(unsigned char* sizeBlock);

	/**
	 * Returns the size of the message that has been written so far in bytes.
	 */
	int getCurrentSize();

	/**
	 * Returns the size of the secret message in bytes.
	 */
	int getMessageSize();

	/**
	 * Returns the number of blocks needed for the conjugation map.
	 */
	int getNumMapBlocks();

private:

	int currentSize; // The current size of the message written so far
	FILE* fpOutput; // File pointer to write to output file
	MapBlock* map; // Array of map blocks for conjugation map
	int mapBlockIdx; // The current map index (for decoding the map)
	int messageSize; // Size of the secret message
	int numMapBlocks; // Number of map blocks needed
	BlockUtility* utility; // Contains functions related to complexity

	/**
	 * Determines if the conjugation bit is set in a size or map block (a block
	 * not accounted for in the conjugation map itself). Returns true if it was
	 * conjugated, false otherwise.
	 */
	bool conjBitSet(unsigned char firstByte);

	/**
	 * Checks the conjugation map to determine if the block at the given index
	 * was conjugated. Returns true if the message bit in the map is 1, false if
	 * it is 0.
	 */
	bool isConjugated(int blockIndex);

	/**
	 * Creates and initializes the number of map blocks needed for the
	 * conujugation map. Returns the number of map blocks needed.
	 */
	int setUpMapBlocks(int size);
};

#endif /* MESSAGEWRITER_H_ */
