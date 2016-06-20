##
# Copyright (c) Project Iota
#
# uCbor is licensed under an Apache license, version 2.0 license.
# All rights not explicitly granted in the Apache license, version 2.0 are reserved.
# See the included LICENSE file for more details.
##

SRCDIR = src
INCDIR = inc
OBJDIR = obj

SRC = $(wildcard $(SRCDIR)/*.c)
INC = $(wildcard $(INCDIR)/*.h)
 
OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = libcbor.a
 
CC := gcc
CFLAGS = -Wall -W -O
LIBS =
LDFLAGS = $(LIBS:%=-l%)

all: $(TARGET)

$(TARGET) : $(OBJ)
	$(AR) rc $@ $<
 
$(OBJ): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<

test: examples/test.o libcbor.a
	$(CC) $(LDFLAGS) $^ -o $@

examples/test.o: examples/test.c
	$(CC) $(CFLAGS) -I$(INCDIR) -c -o $@ $<
 
.PHONY : clean
clean:
	rm -rf $(TARGET) $(OBJDIR) libcbor.a test
