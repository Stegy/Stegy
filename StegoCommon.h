/*
 * StegoCommon.h
 *
 * Contains constants and structures used throughout the program.
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca
 */

#pragma once

#include <stdint.h>

const uint8_t kBlockSize = 8; // Block size in bits (using 8x8 blocks)

/**
 * MapBlock - represents an 8x8 block of the conjugation map
 */
struct MapBlock {
	unsigned char conjBit : 1; // Conjugation bit for this block
	// Remaining first row of block (contains map data)
	unsigned char firstRow : 7;
	// Remaining 7 rows of block
	unsigned char rows[kBlockSize - 1];
	// How much of block is full of map data - index of the next available row
	int fullTo;
};
