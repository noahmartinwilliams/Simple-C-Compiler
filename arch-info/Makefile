INCLUDE=./include
include ../include/config.mk

main: sizes
	echo "done"

%: %.c
	$(CMB)
	./$@

clean:
	rm sizes 2>/dev/null || true
