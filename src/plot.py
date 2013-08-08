# plot the run time v.s. number of threads

import matplotlib.pyplot as plt
import numpy as np
#fname=raw_input("file name: " )
fname="time-10-100.txt"
f= open(fname,"r")
data=[]
for line in f:  
    data.append(int(line)/1000)    

plt.plot(range(1,13,1),data,'-o',ms=10,lw=2,alpha=20,mfc='orange')
xticks= np.arange(1,13,1)
plt.xticks(xticks)
plt.ylabel('time/ms')  #plt.ylabel('time/$\mu s$')
plt.xlabel('num threads')
plt.grid()
plt.title("parallel time")
plt.show()


