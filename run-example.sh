#!/bin/sh

make -C src
./src/example > output

gnuplot > plot.png <<EOF
set terminal png
plot "output" using 1:2 title 'X' with lines, \
     "output" using 1:3 title 'Y' with lines, \
     "output" using 1:4 title 'Z' with lines
EOF

eog plot.png
