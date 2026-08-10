#!/bin/bash
mkdir -p dif
mkdir -p cur
mkdir -p psd

cd psc
FILES=$(ls)
cd ../../../bin

for a in $FILES; do
    bin/pseint ../test/draw/psc/$a --preservecomments --draw ../test/draw/psd/$a
    bin/psdrawE --nogui ../test/draw/psd/$a ../test/draw/cur/$a
done

echo Results:
for a in $FILES; do
    compare -metric PSNR ../test/draw/ref/$a ../test/draw/cur/$a ../test/draw/dif/$a
    echo " - $a"
done
