#!/bin/sh
clang++ -std=c++20 -Wall -Wextra test.cpp -O3 -g -DBBI_LIMIT=8 -o test8.out     &
clang++ -std=c++20 -Wall -Wextra test.cpp -O3 -g -DBBI_LIMIT=16 -o test16.out   &
clang++ -std=c++20 -Wall -Wextra test.cpp -O3 -g -DBBI_LIMIT=32 -o test32.out   &
clang++ -std=c++20 -Wall -Wextra test.cpp -O3 -g -DBBI_LIMIT=64 -o test64.out   &
clang++ -std=c++20 -Wall -Wextra test.cpp -O3 -g -DBBI_LIMIT=128 -o test128.out &
wait
echo 8
test8.out
rm test8.out
echo 16
test16.out
rm test16.out
echo 32
test32.out
rm test32.out
echo 64
test64.out
rm test64.out
echo 128
test128.out
rm test128.out
rm -r *.out.dSYM
