#/bin/sh
g++ -std=c++11 -I./lest/include/lest -iquote test/mocks -iquote . test/test.cpp -o testRunner
./testRunner && echo All tests passed
