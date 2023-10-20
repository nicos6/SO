#!/bin/bash

#grep -E "^[A-Z][a-zA-Z0-9 ,]+\.$"

#grep -vE "(si[ ]*,) | (si[ ][ ]+)"

#grep -v "[n][pb]"

while read linie
do
    echo $linie | grep -E "^[A-Z][a-zA-Z0-9 ,]+\.$" | grep -vE "(si[ ]*,) | (si[ ][ ]+)" | grep -v "[n][pb]"
done
