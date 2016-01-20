#! /bin/bash

CC=gcc
./cc test48-output1.s source-test48.c ../phase-4-backend/libx64-backend.so
./cc test48-output2.s source2-test48.c ../phase-4-backend/libx64-backend.so

as test48-output1.s -o test48-output1.o
as test48-output2.s -o test48-output2.o

$CC test48-output1.o test48-output2.o -o test48
