#!/bin/bash

header="/home/cslaf/godot/godot_headers/"

clang -std=c11 -g -fPIC -c -I $header  test.c -o testlib.os
clang -shared testlib.os -o ../bin/testlib.so

