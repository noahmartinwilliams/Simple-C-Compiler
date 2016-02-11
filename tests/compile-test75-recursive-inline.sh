#! /bin/bash

OUTPUT=$(./cc test75-recursive-inline-output.s source-test75-recursive-inline.c ../phase-4-backend/libx64-backend.so 2>&1 )

[ "$?" = 0 ] && exit 1 ;
(echo "$OUTPUT" | grep "SEGMENTATION") && exit 2;
exit 0;
