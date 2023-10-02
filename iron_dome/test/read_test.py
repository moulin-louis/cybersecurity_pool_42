import multiprocessing

def read_large_file_in_chunks(filename, chunk_size=1024*1024, limit=1024*1024*10):  # Default limit is 10MB
   while True:
        total_read = 0
        f = open('./bigfile', 'rb')
        while total_read < limit:
            chunk = f.read(chunk_size)
            total_read += len(chunk)
            del chunk
        print('One iter of bigfile read')

def spawn_readers(num_processes, filename='bigfile', chunk_size=1024*1024, limit=1024*1024*10):
    processes = []
    for _ in range(num_processes):
        p = multiprocessing.Process(target=read_large_file_in_chunks, args=(filename, chunk_size, limit))
        p.start()
        processes.append(p)

    for p in processes:
        p.join()

spawn_readers(30)