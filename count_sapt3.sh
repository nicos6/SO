#!/bin/bash

if [ "$#" -lt 2 ]
then echo "Prea putine argumente"
fi

output=$2
sum=0

echo "" > $output

for entry in $1/*.txt
do
    nr=$(wc -m < $entry)
    echo $entry $nr >> $output
    sum=`expr $sum + $nr`
done

echo "TOTAL" $sum >> $output

