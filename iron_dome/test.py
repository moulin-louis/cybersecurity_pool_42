import time
import math
import sys
import multiprocessing

def generate_cpu_load(interval=int(sys.argv[1]),utilization=int(sys.argv[2])):
    "Generate a utilization % for a duration of interval seconds"
    start_time = time.time()
    for i in range(0,int(interval)):
        print("About to do some arithmetic")
        while time.time()-start_time < utilization/100.0:
            a = math.sqrt(64*64*64*64*64)
        print(str(i) + ". About to sleep")
        time.sleep(1-utilization/100.0)
        start_time += 1

#----START OF SCRIPT
if __name__=='__main__':
    print("No of cpu:", multiprocessing.cpu_count())
    if len(sys.argv)>2:
        processes = []
        for _ in range (multiprocessing.cpu_count()):
            p = multiprocessing.Process(target =generate_cpu_load)
            p.start()
            processes.append(p)
        for process in processes:
            process.join()
    else:
        print("Usage:\n python %s interval utilization"%__file__)