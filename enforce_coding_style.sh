#!/bin/bash

for basedir in "src"
do
    for file in `ls $basedir/*.{c,h}`
    do
        echo "##### Indenting file $file"
        astyle --style=java --unpad-paren --delete-empty-lines --add-brackets --convert-tabs --align-pointer=type --lineend=linux --suffix=none $file
    done
done
