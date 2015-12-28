#! /bin/bash

CC=gcc
./cc test49-output1.s source-test49.c
./cc test49-output2.s source2-test49.c

as test49-output1.s -o test49-output1.o
as test49-output2.s -o test49-output2.o

$CC test49-output1.o test49-output2.o -o test49
