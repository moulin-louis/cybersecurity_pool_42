#!/bin/bash

if [[ -n $1 ]]; then
    if [[ $1 == "-v" ]]; then
        echo 'Running in verbose mode'
        ./inquisitor "$IP_SERVER" "$MAC_SERVER" "$IP_CLIENT" "$MAC_CLIENT" -v
    else
        echo "Wrong option"
        exit 1
    fi
else
    echo 'Running in non verbose mode'
    ./inquisitor "$IP_SERVER" "$MAC_SERVER" "$IP_CLIENT" "$MAC_CLIENT"
fi