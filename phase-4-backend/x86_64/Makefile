INCLUDE=../../include
include ../../include/config.mk

../libx86_64-backend.so: backend.o jumps.o int-arithmetic.o registers.o agnostic-arithmetic.o statics.o calls.o variables.o globals.o float-arithmetic.o arithmetic.o constants.o
	$(SHARE)

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

