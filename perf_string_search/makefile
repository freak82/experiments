
SHELL := /bin/bash

X3ME_LIBS_PATH=$(HOME)/x3me/x3me_libs/

################################################################################

CPP_FILES=\
	$(wildcard *.cpp)

################################################################################

PRECOMPILED_HEADER=precompiled.h
PRECOMPILED_HEADER_D=$(PRECOMPILED_HEADER).d
PRECOMPILED_HEADER_PCH=$(PRECOMPILED_HEADER).gch

OBJ_FILES=$(CPP_FILES:.cpp=.o)
DEP_FILES=$(CPP_FILES:.cpp=.d)

################################################################################
##-fno-omit-frame-pointer -fsanitize=address -fsanitize=undefined \
##-lasan -lubsan

CXX=g++
CXXFLAGS=-c -MMD -O2 -std='c++17' \
		 -Wall -Wextra -pthread \
		 -isystem./nonius-1.1.2/include/ \
		 -isystem$(X3ME_LIBS_PATH)/ \
		 -isystem$(X3ME_LIBS_PATH)/boost_1_69_0/ \
		 -I ./
LDFLAGS=-pthread \
		-L$(X3ME_LIBS_PATH)/boost_1_69_0/lib/release/lib/

BINARY=perf_string_search.bin

################################################################################

all: $(BINARY)

# the targets are executed by separate/child make processes to ensure
# correct multiprocessor build (-j option)
rebuild:
	$(MAKE) clean
	$(MAKE) all

clean:
	rm -f $(PRECOMPILED_HEADER_PCH) $(PRECOMPILED_HEADER_D) $(DEP_FILES) $(OBJ_FILES) $(BINARY)

################################################################################

# ensure that if some of the included files is changed we are going to rebuild
# the needed file(s)
-include $(PRECOMPILED_HEADER_D)
-include $(DEP_FILES)

# compile the precompiled header if needed
$(PRECOMPILED_HEADER_PCH): $(PRECOMPILED_HEADER)
	$(CXX) -x c++-header $(CXXFLAGS) -o $@ $<

# ensure that the precompiled header will be compiled first and also
# if the precompiled header is changed 
# all object files are going to be compiled again
$(OBJ_FILES): $(PRECOMPILED_HEADER_PCH)

# compile each of the object files if needed
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

# link the all object files into the final executable
$(BINARY): $(OBJ_FILES)
	$(CXX) -o $@ $(OBJ_FILES) $(LDFLAGS)

################################################################################

.PHONY: all rebuild clean
