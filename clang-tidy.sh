#! /bin/bash

find * -iwholename 'include/*.h' -o -iwholename 'src/*.c' | xargs clang-tidy --extra-arg="-Iinclude" --config-file=".clang-tidy"
