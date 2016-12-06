#!/bin/bash
valgrind -v --trace-children=no --leak-check=full --show-reachable=no --track-origins=yes --log-file=valgrind.txt --free-fill=0x0 $*

