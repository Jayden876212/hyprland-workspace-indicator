#! /bin/bash

./configure

iwyu_tool.py -p build
find * -iname '*.h' -o -iname '*.c' | xargs clang-format -i
find * -iwholename 'include/*.h' -o -iwholename 'src/*.c' | xargs clang-tidy -p build
npx prettier . --write
