import os
import threading
import subprocess
import numpy as np

from pathlib import Path

pol = 'y'
p = Path('build/data')
eps = list(p.glob('eps*.h5'))
files = list(p.glob('e{0}*.h5'.format(pol)))
div_files = len(files) - len(files)%4
inc = int(div_files / 4)
remainder = len(list(files))%4

count = 0
if remainder == 0:
    file_arr = np.zeros((4,inc), dtype=object)
    for i in range(4):
        for j in range(inc):
            try:
                file_arr[i,j] = files[count]
                count += 1
            except:
                break

else:
    file_arr = np.zeros((4,inc+1), dtype=object)
    for i in range(4):
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
            command = ["h5topng", "-X", "0.5", "-Y", "0.5", "-Z",
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



