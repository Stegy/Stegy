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
OPTIONS       = -ansi -pedantic-errors -Wall -Wall -Werror -Wextra -o
LINKER_OPT    = -L/usr/lib -lstdc++
OUTPUT_DIR    = bin

all: bitmap_test example cgc_translator_test

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

valgrind_check:
	valgrind --leak-check=full --show-reachable=yes --track-origins=yes -v ./bitmap_test

clean:
	rm -f core *.o *.bak *stackdump *~

#
# The End !
#
