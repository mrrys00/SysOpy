#!/bin/sh

FILE="Times.txt"
MAIN="./thread"
PGMIN="casablanca.ascii.pgm"
PGMOUT="casablanca_inv.pgm"

> $FILE
for var in 1 2 4 8 16
do
    $MAIN $var numbers $PGMIN $PGMOUT >> $FILE
    echo >> $FILE
    $MAIN $var block $PGMIN $PGMOUT >> $FILE
    echo >> $FILE
done
