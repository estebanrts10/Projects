import numpy as np
import math
birds = [2,1,2,1,1,2,2,2,1,1,1,1]
n = len(birds)
def fowl(birds):
    counter=0
    idx = 0
    for bird in range(n):
        if birds[idx] == birds[bird]:
            counter += 1
        else:
            counter -=1
        if counter == -1:
            idx = bird
            counter = 0
    return birds[idx]
majorityBird = fowl(birds)
majList = []
for bird in range(n):
    if majorityBird == birds[bird]:
        majList.append(bird)
print (majList)
