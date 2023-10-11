#!/usr/bin/fish

if set -q argv[1]
    if test "$argv[1]" = "-v"
        echo 'Running in verbose mode'
        ./inquisitor "$IP_SERVER" "$MAC_SERVER" "$IP_CLIENT" "$MAC_CLIENT" -v
    else
        echo "Wrong option"
        exit 1
    end
else
    echo 'Running in non verbose mode'
    ./inquisitor "$IP_SERVER" "$MAC_SERVER" "$IP_CLIENT" "$MAC_CLIENT"
end