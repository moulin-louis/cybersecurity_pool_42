import multiprocessing
import os

def read_large_file_in_chunks():
    chunk_size=1024*1024
    while True:
        total_read = 0
        os.system('echo 1234 | su -c "echo 1 > /proc/sys/vm/drop_caches" > /dev/null')
        with open ("./bigfile", "rb") as f:
            while (byte := f.read(chunk_size)):
                total_read += len(byte)
                del byte

def spawn_readers(num_processes):
    chunk_size=1024*1024
    processes = []
    for _ in range(num_processes):
        p = multiprocessing.Process(target=read_large_file_in_chunks, args=())
        p.start()
        processes.append(p)

    for p in processes:
        p.join()

spawn_readers(30)