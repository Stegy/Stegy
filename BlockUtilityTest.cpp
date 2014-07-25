#include "BlockUtility.h"

#include <iostream>

using namespace std;

int main() {
    /*
    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 0
    0 0 1 1 1 1 1 1
    1 1 1 1 1 1 1 1
    1 1 1 1 1 1 1 1
    1 1 1 1 1 1 1 1
    1 1 1 1 1 1 1 1
    */

	/*
	 * 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
	 */

	/*
	 * aa 54 ab 54 aa 1d aa 1d
	 *
	 * 8a 27 cb 21
< 0000400 c2 30 d8 75 cc 3a d8 75
df 26 8a 21 6f 20 62 65
	 */
	unsigned char block[8] = {0x8A, 0x27, 0xCB, 0x21, 0xc2, 0x30, 0xd8, 0x75};
	unsigned char block2[8] = {0xcc, 0x3a, 0xd8, 0x75, 0xdf, 0x26, 0x8a, 0x21};

	BlockUtility utility;
	unsigned char testBitPlane[8] = {0, 0, 0, 63, 255, 255, 255, 255};
    cout << "Test bit plane1: " << endl;
    utility.printBitPlane(block);
    cout << "original complexity " << utility.getComplexity(block) << endl;
    cout << "Conjugated: " << endl;
    utility.conjugate(block);
    utility.printBitPlane(block);
    cout << "New complexity: " << utility.getComplexity(block) << endl;
    cout << "hex values: " << endl;
    for (int i = 0 ; i < 8; i++ ) {
    	printf("%x\n", (int) block[i]);
    }
    utility.printBitPlane(block2);
    cout << "original complexity " << utility.getComplexity(block2) << endl;
    cout << "Conjugated: " << endl;
    utility.conjugate(block2);
    utility.printBitPlane(block2);
    cout << "New complexity: " << utility.getComplexity(block2) << endl;
    cout << "hex values: " << endl;
    for (int i = 0 ; i < 8; i++ ) {
    	printf("%x\n", (int) block2[i]);
    }
    return 0;

    utility.setComplexityThreshold(0.2);
    cout << "Bit plane complexity: " << utility.getComplexity(testBitPlane) << endl;
    bool complex = utility.isComplex(testBitPlane);
    cout << "Found complex: " << complex << endl;
    utility.conjugate(testBitPlane);
    cout << "Conjugated: " << endl;
    utility.printBitPlane(testBitPlane);
    cout << "Bit plane complexity: " << utility.getComplexity(testBitPlane) << endl;
    utility.conjugate(testBitPlane);
    cout << "Conjugated back: " << endl;
    cout << "Bit plane complexity: " << utility.getComplexity(testBitPlane) << endl;
    utility.printBitPlane(testBitPlane);

}
