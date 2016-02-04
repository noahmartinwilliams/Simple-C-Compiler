#! /bin/bash

EXPORT=../include/generator/backend-exported.h
echo "#ifndef __GENERATOR_BACKEND_EXPORTED_H__" >$EXPORT
echo "#define __GENERATOR_BACKEND_EXPORTED_H__" >>$EXPORT
echo "#include \"types.h\"" >>$EXPORT
echo "#include \"generator/types.h\"" >>$EXPORT
echo "#include <stdio.h>" >>$EXPORT

LOADER=../include/loader-macro.h
echo "#ifndef __LOADER_MACRO_H__" >$LOADER
echo "#define __LOADER_MACRO_H__" >>$LOADER
echo -n "#define load_obj3 " >> $LOADER

EXPORT_SOURCE=exported.c
echo "#include <stdio.h>" >$EXPORT_SOURCE
echo "#include \"types.h\"" >>$EXPORT_SOURCE
echo "#include \"generator/types.h\"" >>$EXPORT_SOURCE


cat list-of-exported-functions.txt | while read FUNC;
do
	ID="[_a-zA-Z][_a-zA-Z0-9]*"
	MODS='\('"$ID"'[[:space:]]\+\)*'
	MS='[[:space:]]\+' #Multi-Space
	AS='[[:space:]]*' #Any number of spaces
	SYMBOL='^'"$MODS"'\('"$ID$MS"'\**'"$AS"'\)\('"$ID"'\)'"$AS"'\((.*);\)'
	SYMBOL2='^'"$MODS"'\('"$ID$AS"'\**'"$MS"'\)\('"$ID"'\)'"$AS"'\((.*);\)'
	echo "$FUNC" | sed -e 's/'"$SYMBOL"'/\1\2(*\3)\4/g' \
	-e 's/'"$SYMBOL2"'/\1\2(*\3)\4/g' \
	-e 's/^\(.*\)$/extern \1/g' >>$EXPORT

	echo "$FUNC" | sed -e 's/'"$SYMBOL"'/\1\2(*\3)\4/g' \
	-e 's/'"$SYMBOL2"'/\1\2(*\3)\4/g' \
	-e 's/^\(.*\)$/\1/g' >>$EXPORT_SOURCE

	echo "$FUNC" | sed -e 's/'"$SYMBOL"'/load_obj2(\&\3, "\3"); \\/g' \
	-e 's/'"$SYMBOL2"'/load_obj2(\&\3, "\3"); \\/g'  >>$LOADER


done

echo "#endif" >>$EXPORT
echo "0" >>$LOADER
echo "#endif" >>$LOADER
