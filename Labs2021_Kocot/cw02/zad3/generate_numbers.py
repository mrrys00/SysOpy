from random import randint

with open('data.txt', 'w') as dane:
    for i in range(42000):
        if i % 472 == 0:
            dane.write(str(randint(0, 44720) ** 2) + '\n')
        else:    
            dane.write(str(randint(0, 2147483647)) + '\n')
