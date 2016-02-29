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
clean: 
	$(MAKE) -C x64/ clean
	rm comp.tab.c 2>/dev/null || true
	rm comp.y 2>/dev/null || true
	rm *.o 2>/dev/null || true
	rm lex.yy.c 2>/dev/null || true
	rm *.so 2>/dev/null || true
	rm comp.output 2>/dev/null || true
	rm *.s 2>/dev/null || true
	rm main 2>/dev/null || true
	rm include/comp.tab.h 2>/dev/null || true
	rm cc 2>/dev/null || true
	rm *.a 2>/dev/null || true

