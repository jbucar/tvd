#!/bin/bash
dump_syms $1 > $2.sym
sum=`head -n1 $2.sym | awk '{print $4}'`
mkdir -p symbols/$2/$sum
mv $2.sym symbols/$2/$sum
