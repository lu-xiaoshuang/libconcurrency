#
# Copyright (C) 2018 Xiaoshuang LU
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
#    without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

include ../build/makefile.define

INCLUDE = ../include
LIBRARY = ../library

INCLUDE_PATH_FLAGS += -I $(INCLUDE)
LIBRARY_PATH_FLAGS += -L $(LIBRARY)

TEST_DIRECTORY = ../test/

DEFAULT = default
CLEAN = clean

.PHONY: $(DEFAULT) $(CLEAN)

HEADER = \
    $(TEST_DIRECTORY)test.h

SOURCE = \
    $(TEST_DIRECTORY)test.c \
    $(TEST_DIRECTORY)test.c

OBJECT = \
    test.o \
    test.o

$(DEFAULT): $(HEADER) $(SOURCE) $(OBJECT)
	$(CC) $(OBJECT) $(LIBRARY_PATH_FLAGS) -llist$(WORD_SIZE) -o test

%.o: $(TEST_DIRECTORY)%.c
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $@
	$(CC) $(CFLAGS) $(INCLUDE_PATH_FLAGS) -c $< -o $@

$(CLEAN):
	$(RM) *.o test
