INCLUDE=./include
include include/config.mk


main: main.o globals.o handle.o lexer.a generator.o optimization-globals.o loader.o exported.o utilities.o
	$(CMBPROG) -ldl
	chmod +x $@
	cp $@ cc

exported.o: phase-4-backend/*
	$(MAKE) -C phase-4-backend/ ../exported.o


utilities.o: phase-0-utilities/*
	$(MAKE) -C phase-0-utilities/ ../utilities.o

loader.o: phase-4-backend/*
	$(MAKE) -C phase-4-backend/ ../loader.o

.PHONY:
phase-4-backend/arch: phase-4-backend/* phase-4-backend/*/*
	$(MAKE) -C phase-4-backend/ arch

generator.o: phase-3-generator/*
	$(MAKE) -C phase-3-generator/ ../generator.o

handle.o: phase-2-parser/*
	$(MAKE) -C phase-2-parser/ ../handle.o

lexer.a: phase-1-lexer/* handle.o
	$(MAKE) -C phase-1-lexer/ ../lexer.a

%.o: %.c
	$(CMP)

.PHONY:
main2: main phase-4-backend/arch
	echo "done"

.PHONY:
test: main_debug
	cp main tests/cc
	$(MAKE) -s -C tests/ test

.PHONY:
main_debug:
	$(MAKE) DEBUG=1 main2

.PHONY:
test%: main_debug
	cp main tests/cc
	$(MAKE) -s -C tests/ $@

.PHONY:
clena: 
	$(MAKE) clean

.PHONY:
clean: 
	rm *.a 2>/dev/null || true
	rm *.o 2>/dev/null || true
	rm cc 2>/dev/null || true
	rm main 2>/dev/null || true
	rm lib*.so 2>/dev/null || true
	$(MAKE) -C tests/ clean
	$(MAKE) -C phase-0-utilities/ clean
	$(MAKE) -C phase-1-lexer/ clean
	$(MAKE) -C phase-2-parser/ clean
	$(MAKE) -C phase-3-generator/ clean
	$(MAKE) -C phase-4-backend/ clean
