import random
n = input()
n = int(n)
arr = []
for i in range(n):
    arr.append(int(input()))
    #print(random.randint(-n,n))
arr.sort()
for i in range(n):
    print(arr[i])
