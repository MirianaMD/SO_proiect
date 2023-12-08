#!/bin/bash

functie()
{
    if [ "$#" -ne 1 ]; then
	echo "Nr incorect de argumente!"
	exit 1
    fi
    caracter=$1;
    count=0;
    while  read -r line;
    do
	if((echo "$line" | grep -E "^[A-Z][A-Za-z0-9 ,.!?]*[.!?]$" | grep -v ",si" | grep -E -c $caracter)>0) 
	then
            ((count++))
	fi
    done
    echo "$count"
}
functie $1
