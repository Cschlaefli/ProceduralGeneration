#!/bin/bash

clang -std=c11 -fPIC -c -I /home/cslaf/godot/godot_headers/ test.c -o testlib.os
clang -shared testlib.os -o ../bin/testlib.so

