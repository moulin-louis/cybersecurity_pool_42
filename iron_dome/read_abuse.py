import threading

def read_dev_zero_in_chunks(chunk_size=1024*1024, limit=1024*1024*10):  # Default limit is 10MB
    total_read = 0
    f = open('./bigfile', 'rb')
    while total_read < limit:
        chunk = f.read(chunk_size)
        total_read += len(chunk)
        # Simulate processing, or simply discard the chunk to abuse disk reads
        del chunk

def spawn_readers(num_threads=5, chunk_size=1024*1024, limit=1024*1024*10):
    threads = []
    while True:
        for _ in range(num_threads):
            t = threading.Thread(target=read_dev_zero_in_chunks, args=(chunk_size, limit))
            t.start()
            threads.append(t)
        for t in threads:
            t.join()
            print('10 MB read')


spawn_readers(num_threads=10)