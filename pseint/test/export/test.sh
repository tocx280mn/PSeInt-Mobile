#!/bin/sh
if [ "$1" = "" ]; then 
  echo "Use: $0 langcode"
else
  LANGCODE=$1
  if ! make -C ../../psexport ARCH=lnx; then exit 1; fi
  ebin=../../../bin/bin/psexport
  
  echo LANG: $LANGCODE
  if ! make -C ../../pseint ARCH=lnx; then exit 1; fi

  if ! test -e $LANGCODE; then
    mkdir $LANGCODE
  fi
  cd psc >/dev/null
  
  ibin=../../../bin/bin/pseint
  mkdir -p ../temp
  
  for A in *.psc; do
  
    echo "=== $A"
  
    B=$(echo $A|sed 's/psc/psd/')
    C=$(echo $A|sed 's/psc/'$LANGCODE'/')
    
    $ibin --allow_dinamyc_dimensions=1 --nouser $A --preservecomments --export ../temp/$B
    
    if ! test -e ../$LANGCODE/$C; then
      $ebin --for-testing --lang=$LANGCODE ../temp/$B ../$LANGCODE/$C >/dev/null
    else
      $ebin --for-testing --lang=$LANGCODE ../temp/$B ../temp/$C >/dev/null
      diff --strip-trailing-cr ../temp/$C ../$LANGCODE/$C
    fi
  done
fi  
