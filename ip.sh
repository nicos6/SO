#!/bin/bash

while read line
do
    # echo $linie | grep -E "^[0-9]{1,3}\.{3}[0-9]{1,3}$" | grep -E -v "2[6-9][0-9]" | grep -E -v "[3-9][0-9][0-9]" | grep -E -v "25[6-9]"
    echo $linie | grep -E "^([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5]\.){3}
done