#! /bin/bash

CC=gcc
./cc test48-output1.s source-test48.c $(./get-arc-name.sh)
./cc test48-output2.s source2-test48.c $(./get-arc-name.sh)

as test48-output1.s -o test48-output1.o
as test48-output2.s -o test48-output2.o

$CC test48-output1.o test48-output2.o -o test48
