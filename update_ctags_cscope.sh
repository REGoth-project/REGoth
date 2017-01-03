#!/bin/bash
#gentagdb database build script
find -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' -o -name '*.cxx' >cscope.files
sort cscope.files >cscope.files.sorted
mv cscope.files.sorted cscope.files
cscope -b
ctags -L cscope.files
