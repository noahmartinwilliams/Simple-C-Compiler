INCLUDE=../include
include ../include/config.mk

../generator.a: misc.o globals.o expr.o statem.o 
	$(AR)

statem.o: statem.c statems/*
	$(CC) -I ./statems -c $< -o $@
	
%.o: %.c
	$(CMP)

.PHONY:
clean: clean_intern
	echo "phase 3 clean"
