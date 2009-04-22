#!/bin/sh

set -e

make -C src
./src/example > output

gnuplot > plot.png <<EOF
set terminal png
plot "output" using 1:2 title 'X' with lines, \
     "output" using 1:3 title 'Y' with lines, \
     "output" using 1:4 title 'Z' with lines, \
     "output" using 1:5 title 'S1' with lines, \
     "output" using 1:6 title 'S2' with lines
EOF

eog plot.png
