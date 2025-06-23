import os
import threading
import subprocess
import numpy as np
import h5py

from pathlib import Path

pol = 'y'
p = Path('build/data')
eps = list(p.glob('eps*.h5'))
files = list(p.glob('e{0}*.h5'.format(pol)))

sample = 7
E_max_arr = np.zeros(sample)
E_min_arr = np.zeros(sample)
for i in range(sample):
    f = h5py.File(files[i], 'r')
    dset = f['e{0}.r'.format(pol)]
    E_max_arr[i] = np.max(dset)
    E_min_arr[i] = np.min(dset)


E_max = np.max(E_max_arr)
E_min = np.min(E_min_arr)

thread_num = 8
div_files = len(files) - len(files)%thread_num
inc = int(div_files / thread_num)
remainder = len(list(files))%thread_num

count = 0
if remainder == 0:
    file_arr = np.zeros((thread_num,inc), dtype=object)
    for i in range(thread_num):
        for j in range(inc):
            try:
                file_arr[i,j] = files[count]
                count += 1
            except:
                break

else:
    file_arr = np.zeros((thread_num,inc+1), dtype=object)
    for i in range(thread_num):
        for j in range(inc+1):
            try:
                file_arr[i,j] = files[count]
                count += 1
            except:
                break


def create_png ( files ):
    for i in range(len(files)):
        if files[i] != 0:
            file_num = os.fspath(files[i])[14:-3]
            command = ["h5topng", "-X", "0.5", "-Y", "0.5", "-Z", "-m",
                       "{0}".format(E_min), "-M", "{0}".format(E_max),
                       "-o", "{0}/overlay-{1}.png".format(p, file_num),
                       "-c", "RdBu", "{0}:e{1}.r".format(files[i], pol),
                       "-A", "{0}".format(eps[0]), "-a", "-yellow:0.8"]
            subprocess.run(command)
    return 0

threads = []
for i in range(file_arr.shape[0]):
    t = threading.Thread(target=create_png, args=[file_arr[i,:]])
    threads.append(t)

for t in threads:
    t.start()

for t in threads:
    t.join()



