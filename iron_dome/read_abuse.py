import threading

def read_dev_zero_in_chunks(chunk_size=1024*1024, limit=1024*1024*10):
    while True:
        with open('/dev/zero', 'rb') as f:
                chunk = f.read(chunk_size)
                del chunk
        print('read done')


num_threads=10
threads = []
for _ in range(num_threads):
    t = threading.Thread(target=read_dev_zero_in_chunks, args=(chunk_size, limit))
    t.start()
    threads.append(t)

# Wait for all threads to complete
for t in threads:
    t.join()
