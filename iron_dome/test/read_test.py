import multiprocessing
import os

def read_large_file_in_chunks():
    chunk_size= 1024*1024
    while True:
        os.system('echo 1234 | su -c "echo 1 > /proc/sys/vm/drop_caches"')
        total_read = 0
        with open ("./bigfile", "rb") as f:
            while (byte := f.read(chunk_size)):
                total_read += len(byte)
                del byte
        print('bytes read:', total_read)

read_large_file_in_chunks()