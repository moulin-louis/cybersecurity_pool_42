while (True):
    f = open("./big_file2")
    for line in f.readline():
        print(len(line))
    f.close()
