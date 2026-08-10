#!/bin/bash

echo -n uname: > "$HOME/pseint-log.txt"
uname -a >> "$HOME/pseint-log.txt"

echo -n version: >> "$HOME/pseint-log.txt"
cat version >> "$HOME/pseint-log.txt"

echo "" >> "$HOME/pseint-log.txt"

echo "ldd wxPSeInt" >> "$HOME/pseint-log.txt"
ldd wxPSeInt >> "$HOME/pseint-log.txt"

echo "ldd wxPSeInt.bin" >> "$HOME/pseint-log.txt"
ldd wxPSeInt.bin >> "$HOME/pseint-log.txt"

echo "ldd pseint" >> "$HOME/pseint-log.txt"
ldd pseint>> "$HOME/pseint-log.txt"

echo "ldd psdrawE" >> "$HOME/pseint-log.txt"
ldd psdrawE>> "$HOME/pseint-log.txt"

echo "ldd psdraw3" >> "$HOME/pseint-log.txt"
ldd psdraw3>> "$HOME/pseint-log.txt"

echo "ldd pseval" >> "$HOME/pseint-log.txt"
ldd pseval>> "$HOME/pseint-log.txt"

echo "ldd psexport" >> "$HOME/pseint-log.txt"
ldd psexport>> "$HOME/pseint-log.txt"

echo "ldd psterm" >> "$HOME/pseint-log.txt"
ldd psterm>> "$HOME/pseint-log.txt"

echo "ldd updatem.bin" >> "$HOME/pseint-log.txt"
ldd updatem.bin>> "$HOME/pseint-log.txt"

./wxPSeInt --logger "$HOME/pseint-log.txt"
