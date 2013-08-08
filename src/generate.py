# generate a random matrix and save to random.txt

import random

w=int(raw_input('width: '))
h=int(raw_input('height: '))
dims= "width: %d\nheight: %d" % (w,h)
f = open("random.txt", "w")
f.write(dims)

for j in range(h):
    f.write("\n")
    for i in range(w):
        f.write("%d " % (random.randint(0,1)) )
f.close()
        
