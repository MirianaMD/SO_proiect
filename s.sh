#!/bin/bash

functie()
{
    caracter=$1;
    while  read -r line;
    do
	echo "$line" | grep -E "^[A-Z][A-Za-z0-9 ,.!?]*[.!?]$" | grep -v ",si" | grep -E -c $caracter 
	if [ $? -eq 0 ]
	then
            ((count++))
	fi
    done
    echo $count
}
functie $1