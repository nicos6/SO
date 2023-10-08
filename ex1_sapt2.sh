#!/bin/bash

if test $# -lt 3
then echo "Numarul de argumente este gresit"
fi

file=$1
dir=$2

shift 2
count=0
sum=0

for arg
do
    if test $arg -gt 10
    then (( count ++ ))
    fi
    sum=`expr $sum + $arg`
done

echo "Numarul de nr > 10: $count"
echo "Suma este: $sum"

cifresuma=0
cifresuma=$(echo $sum | wc -L)
echo "Suma are $cifresuma cifre"

if [ -f $file ]
then echo "Fisierul transmis ca parametru este obisnuit" &&
	echo "Suma: $sum" > $file && echo "Count: $count" >> $file
fi

linii=$(cd $dir | ls *.txt)
for linie in $linii
do
    cat $linie
done

 


