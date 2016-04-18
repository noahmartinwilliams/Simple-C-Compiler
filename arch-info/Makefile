INCLUDE=./include
include ../include/config.mk

main: sizes
	echo "done"

%: %.c
	$(CMBPROG)
	./$@

clean:
	rm sizes 2>/dev/null || true
