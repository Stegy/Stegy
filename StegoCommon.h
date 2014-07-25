/*
 * StegoCommon.h
 *
 *  Created on: Jul 3, 2014
 *      Author: Rebecca
 */

#pragma once

const int kBlockSize = 8;

// Map block
struct MapBlock {
	unsigned char conjBit : 1;
	unsigned char firstRow : 7;
	unsigned char rows[kBlockSize - 1];
	int fullTo; // Index of the next available row after map data
};
