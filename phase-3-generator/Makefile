INCLUDE=../include
include ../include/config.mk

../generator.a: misc.o globals.o expr.o statem.o 
	$(AR)

statem.o: statem.c statems/*
	$(CC) -I ./statems -c $< -o $@
	
%.o: %.c
	$(CMP)

.PHONY:
clean: 
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

