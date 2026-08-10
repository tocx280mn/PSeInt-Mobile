#!/bin/bash
if  [ "$1" = "" ]; then
  make -f Makefile ARCH=lnx -C ../../pseint -j 2 || exit 1
  BIN=
else
  BIN="-bin $1"
fi

echo ""
for A in *.psc; do 
    echo -n -e $A"              " \\r; 
    ./run.sh $BIN $(echo $A | sed 's/.psc//');
done
echo "done                           "
