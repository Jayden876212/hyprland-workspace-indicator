#! /bin/bash

find * -iwholename 'include/*.h' -o -iwholename 'src/*.c' | xargs clang-tidy -p build

