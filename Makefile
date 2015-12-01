# $Id: Makefile,v 1.2 2003/07/21 13:12:49 nearora Exp $

CC = gcc
CFLAGS = -Wall -pthread
TARGETS = main

# rules

# default

# targets

all: $(TARGETS)

clean:
	rm -fr $(TARGETS) *~ *.o
