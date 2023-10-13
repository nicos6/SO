#!/bin/bash

if [ "$#" -lt 2 ]
then echo "Prea putine argumente"
fi

for entry in $1/*.txt
do chmod +$2 $entry
done

for entry in $1/*
do
    if [ -d $entry ]
    then bash $0 $entry $2
    fi
done
