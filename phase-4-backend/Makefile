INCLUDE=../include
include ../include/config.mk

../loader.o: loader.c
	$(CMP)

../exported.o: exported.o
	cp $^ $@

%.o: %.c
	$(CMP)

exported.c: list-of-exported-functions.txt
	./update-header.sh

libx64-backend.so: x64/*
	$(MAKE) -C x64/ ../libx64-backend.so

.PHONY:
clean: clean_intern
	$(MAKE) -C x64/ clean
	echo "phase 4 clean"
