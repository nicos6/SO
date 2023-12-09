#!/bin/bash

if [ "$#" -ne 1 ]
then echo "Numar gresit de argumente"
exit 1
fi

counter=0
caracter=$1

while read linie
do
    if ( echo $linie | (grep -E "^[A-Z][a-zA-Z0-9 ,\.!?]*[?!\.]$" | grep -vE "(,[ ]*si)" | grep -q "$caracter") ) #am pus q la ultimul grep ca sa nu mai afiseze propozitia
    then ((counter++))
    fi
done

echo $counter