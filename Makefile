#
# Platform Independent
# Bitmap Image Reader Writer Library
# By Arash Partow - 2002
#
# URL: http://partow.net/programming/bitmap/index.html
#
# Copyright Notice:
# Free use of this library is permitted under the
# guidelines and in accordance with the most
# current version of the Common Public License.
# http://www.opensource.org/licenses/cpl1.0.php
#

COMPILER      = -c++
OPTIONS       = -ansi -Wall -Wall -o
LINKER_OPT    = -L/usr/lib -lstdc++
OUTPUT_DIR    = bin

all: bitmap_test example cgc_translator_test message_manager_test stegy

bitmap_test: bitmap_test.cpp bitmap_image.hpp
	$(COMPILER) $(OPTIONS) $(OUTPUT_DIR)/bitmap_test bitmap_test.cpp $(LINKER_OPT)

example: example.cpp
	$(COMPILER) $(OPTIONS) $(OUTPUT_DIR)/example example.cpp $(LINKER_OPT)
	
CGCTranslator.o: 	CGCTranslator.cpp CGCTranslator.h
	$(COMPILER) -Wall -g -c CGCTranslator.cpp
	
TestCGCTranslator.o : TestCGCTranslator.cpp CGCTranslator.h
	$(COMPILER) -Wall -g -c TestCGCTranslator.cpp
	
cgc_translator_test: CGCTranslator.o TestCGCTranslator.o
	$(COMPILER) -g CGCTranslator.o TestCGCTranslator.o $(LINKER_OPT) -lm -o $(OUTPUT_DIR)/TestCGCTranslator
	
MessageReader.o: MessageReader.cpp MessageReader.h StegoCommon.h
	$(COMPILER) -Wall -g -c MessageReader.cpp
	
MessageWriter.o: MessageWriter.cpp MessageWriter.h StegoCommon.h
	$(COMPILER) -Wall -g -c MessageWriter.cpp
	
MessageManagerTest.o: MessageManagerTest.cpp
	$(COMPILER) -Wall -g -c MessageManagerTest.cpp
	
message_manager_test: MessageManagerTest.o MessageReader.o MessageWriter.o BlockUtility.o CGCTranslator.o
	$(COMPILER) -g MessageReader.o MessageWriter.o MessageManagerTest.o BlockUtility.o CGCTranslator.o $(LINKER_OPT) -lm -o $(OUTPUT_DIR)/MessageManagerTest

BlockUtility.o: BlockUtility.cpp BlockUtility.h
	$(COMPILER) -Wall -g -c BlockUtility.cpp
	
Main.o: Main.cpp 
	$(COMPILER) -Wall -g -c Main.cpp

stegy: Main.o BlockUtility.o CGCTranslator.o MessageReader.o MessageWriter.o
	$(COMPILER) -g Main.o MessageReader.o MessageWriter.o BlockUtility.o CGCTranslator.o $(LINKER_OPT) -lm -o stegy

valgrind_check:
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./bitmap_test

clean:
	rm -f core *.o *.bak *stackdump *~

#
# The End !
#
