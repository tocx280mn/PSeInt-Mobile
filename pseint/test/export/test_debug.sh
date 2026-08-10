#!/bin/sh

if [ "$2" = "" ]; then 
  echo "Use: $0 bin_abs_path langcode"
else
  LANGCODE=$2
  ebin=$1
  
  echo LANG: $LANGCODE

  cd psc
  
  ibin=../../../bin/pseint
  
  for A in *.psc; do
  
    echo "=== $A"
  
    B=$(echo $A|sed 's/psc/psd/')
    C=$(echo $A|sed 's/psc/'$LANGCODE'/')
    
    $ibin --allow_dinamyc_dimensions=1 --nouser $A --preservecomments --export ../temp/$B
    
    $ebin --for-testing --lang=$LANGCODE ../temp/$B ../temp/$C >../temp/$C
    if ! diff --strip-trailing-cr ../temp/$C ../$LANGCODE/$C; then read; fi
  done
fi  
read