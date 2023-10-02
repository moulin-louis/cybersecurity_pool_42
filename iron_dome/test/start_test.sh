#!/usr/bin/fish
function shutdown
    echo "Killing crypto test..."
    pkill -f crypto_test
    echo "Killing read test..."
    pkill -f read_test.py
    echo "Killing entropy test..."
    pkill -f entropy_test.sh
    pkill -f entropy_test
    rm -rf file_test
    rm -rf crypto_test
    echo "All cleanup done."
end

echo 'Creating bigfile for testing...'
if  [ ! -f "./bigfile" ]; then
    dd if=/dev/urandom of=bigfile bs=1G count=1
;end

echo 'Setup for SIGINT'
trap shutdown SIGINT

gcc -o crypto_test crypto_test.c -lcrypto
echo 'Running crypto_test'
./crypto_test &
echo 'Running read_test'
python3 read_test.py &
echo 'Running entropy_test'
./entropy_test.sh &
while true; ;end