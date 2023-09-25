chunk_size = 1024
with open("/dev/random") as f:
    while True:
        chunk = f.read(chunk_size)
        if not chunk:
            break
        del chunk