#! /usr/bin/python3

file = open('train_triplets.txt','r')

output = []

for line in file:
    val = line.split()[0] 
    if val not in output:
        output.append(val)

