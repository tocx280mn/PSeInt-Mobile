#!/bin/bash
if [ "$1" = "" ]; then
  echo use: $1 [-bin <path_to_pseint_bin>] testname
else
  if [ "$1" = "-bin" ]; then
    PSEINT=$2
    shift; shift; 
  else
    PSEINT=../../bin/bin/pseint
  fi
  
  if ! test -e $1.psc; then
    echo File $1.psc not found
    exit 1
  fi
  if test -e $1.arg; then
    ARGS=$(cat $1.arg)
  else
    ARGS=
  fi
  if test -e $1.out; then
    if ! $PSEINT --fortest --rawerrors --nouser --noinput $ARGS $1.psc | diff - $1.out &>/dev/null; then
	echo $1 1>&2
	$PSEINT --fortest --rawerrors --nouser --noinput $ARGS $1.psc | diff - $1.out 1>&2
	echo "" 1>&2
    fi
  else
      make ARCH=lnx -C ../../pseint -j 2 || exit 1
      cat $1.psc
      echo === running, args=$ARGS
      $PSEINT --fortest --rawerrors --nouser --noinput $ARGS $1.psc > $1.out
      cat $1.out
  fi
fi
