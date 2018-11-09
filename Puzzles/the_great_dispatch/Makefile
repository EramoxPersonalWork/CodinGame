#!/bin/sh

INPUT=input.txt
ADAPT_IN=adapt_in
PROG=great_dispatch
ADAPT_OUT=adapt_out

CPPFLAGS=-Wall -Wextra -Werror -std=c++11

.PHONY: all

all: build

#$(ADAPT_IN): $(ADAPT_IN).o
#$(PROG): $(PROG).o
#$(ADAPT_OUT): $(ADAPT_OUT).o

build: $(ADAPT_IN) $(PROG) $(ADAPT_OUT)

test: $(INPUT) build
	cat $(INPUT) | ./$(ADAPT_IN) | ./$(PROG) | ./$(ADAPT_OUT)
