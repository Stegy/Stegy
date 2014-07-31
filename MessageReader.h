/*
 * MessageReader.h
 *
 * Contains the MessageReader class definition.
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca Strautman
 */

#ifndef MESSAGEREADER_H_
#define MESSAGEREADER_H_

#include "BlockUtility.h"
#include "StegoCommon.h"

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

/**
 * Used to read a secret message in such a way that it can be embedded using
 * BPCS steganography. Reads an entire message from a file into a byte array,
 * finds the size of the message, and creates the necessary header information
 * (size and conjugation map blocks) to decode the message after embedding.
 */
class MessageReader {

public:
	/**
	 * Constructor reads the secret message file with the given file name into
	 * memory. It gets the size and constructs a conjugation map of the
	 * necessary size. It also requires a BlockUtility object with the
	 * complexity threshold set.
	 */
	MessageReader(string fileName, BlockUtility* utility);

	/**
	 * Frees the memory for the secret message and the conjugation map.
	 */
	~MessageReader();

	/**
	 * Returns the size of the message that has been read by the user so far.
	 * (E.g. if the user has read 20 bytes, returns 20).
	 */
	int getCurrentSize();

	/**
	 * Gets the next 8 bytes (8x8 block) from the secret message, if possible,
	 * and stores them in the passed buffer. Assumes there is enough room in the
	 * buffer. If fewer than readSize bytes are left in the message, it stores
	 * these bytes. It returns true if the entire message has been retrieved.
	 * This method returns the message data as is - it is up to the caller to
	 * conjugate and set the conjugation bit in the map if necessary.
	 */
	bool getNext(unsigned char* buffer);

	/**
	 * Stores the next map block that should be embedded in the passed buffer,
	 * if another map block exists. This method should be called when all of
	 * the secret message has been embedded and the corresponding map bits have
	 * been set. Returns true when all of the map blocks have been retrieved
	 * (should not be called again after this point).
	 */
	bool getNextMapBlock(unsigned char* block);

	/**
	 * Returns the number of blocks needed for the conjugation map. When
	 * embedding the message, space for these should be reserved initially, and
	 * the completed map blocks should be embedded at the end.
	 */
	int getNumMapBlocks();

	/**
	 * Returns the size of the secret message in bytes.
	 */
	int getSize();

	/**
	 * Returns the 8-byte block containing the file size, conjugated if
	 * necessary. The most significant bit of the first byte in the array is
	 * the conjugation bit. Assumes the size fits in 63 bits.
	 */
	void getSizeBlock(unsigned char* result);

	/**
	 * Sets the bit in the conjugation map for the given block. The blockIndex
	 * passed in is the index of the secret block being embedded (starting with
	 * 0 for the first block embedded), and the parameter "conjugated" should
	 * be true if the block was conjugated and false otherwise. It sets the
	 * corresponding bit in the map to 1 if conjugated is true, and 0 if
	 * conjugated is false.
	 */
	void setMapBit(int blockNumber, bool value);

private:
	unsigned char* fileBuffer; // Contains the message file in memory
	FILE* fpInput; // File pointer to read message file
	MapBlock* map; // Array of map blocks for conjugation map
	uint64_t mapIndex; // The current map block index
	uint64_t messageIndex; // The current index in the file buffer
	uint64_t numMapBlocks; // Number of map blocks needed
	uint64_t size; // Size of message file
	BlockUtility* utility; // Contains functions related to complexity

	/**
	 * Builds the conjugation map. This includes 1 bit for each block of message
	 * data. If there is space remaining in the last map block, the beginning of
	 * the secret message data is stored in the remaining bytes.
	 */
	void buildMap();
};



#endif /* MESSAGEREADER_H_ */
