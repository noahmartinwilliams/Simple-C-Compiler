INCLUDE=../include
include ../include/config.mk

../generator.a: misc.o globals.o expr.o statem.o
	$(AR)

.PHONY:
clean: clean_intern
	echo "phase 3 clean"
