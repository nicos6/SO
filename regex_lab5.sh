#!/bin/bash

if [ "$#" -lt 1 ]
then echo "Prea putine argumente"
fi

res=0
nr=0
output=$2
count=0
coun2=0


for entry in $1/*
do
    if [ -f $entry ]
       then
       res=$(cat $entry | grep -E "^(0[1-9]|[1-2][0-9]|3[0-1])\-(0[1-9]|1[0-2])\-[0-9]{4}$")
       nr=$(echo $res | wc -L)
       if [ $nr -gt 0 ]
       then echo $entry >> $output
       fi

       elif [ -L $entry ]
       then count=`expr $count + 1`

       elif [ -d $entry ]
    then count2=`bash $0 $entry $2`
	 count=`expr $count + $count2`
    fi
    
done

echo $count

