#!/bin/sh

set -e

make -C src
./src/example > output

gnuplot > plot.png <<EOF
set terminal png
plot "output" using 1:2 title 'Theta'  with lines, \
     "output" using 1:3 title 'Vx'     with lines, \
     "output" using 1:4 title 'Vy'     with lines, \
     "output" using 1:5 title 'R'      with lines
EOF

eog plot.png
