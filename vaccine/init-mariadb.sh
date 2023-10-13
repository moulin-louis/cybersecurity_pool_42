#!/bin/bash

echo "STARTING SCRIPT"

# Wait for MariaDB to start
sleep 5s

mysql <<EOF
create user 'test'@'%' identified by '1234';
grant all privileges on *.* to 'test'@'%' with grant option;
flush privileges;
EOF

echo "DONE"