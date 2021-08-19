#!/bin/env python3

import sys

if( len(sys.argv) < 3 ):
    print('missing command line argument!')
    exit()

def readFasta(filename):
    with open(filename) as f:
        f.readline();
        string = f.read().replace('\n','')
    return string


# Reading BLOSUM62 matrix
BLOSUM62=[]

with open("blosum62","r") as f:
    for line in f.readlines():
        BLOSUM62.append( list(map(int,line.split())) )

am =  [ 'A',  'R',  'N',  'D',  'C',  'Q',  'E',  'G',  'H',  'I',  'L',  'K', \
             'M',  'F',  'P',  'S',  'T',  'W',  'Y',  'V' ]; 

# Reading fasta files
s1 = readFasta(sys.argv[1])
s2 = readFasta(sys.argv[2])
l1 = len(s1)
l2 = len(s2)

# Scoring
score = []
trace = []

# Initialization
for i in range(l2+1):
    score.append([])
    trace.append([])
    if(i == 0):
        score[0].append(0)
        trace[0].append('0')
        for j in range(1,l1+1):
            score[0].append(score[0][j-1]-10)
            trace[0].append('l')
    else:
        score[i].append(score[0][j-1]-10)
        trace[i].append('u')

# Scoring
g = -10
for i in range(1,l2+1):
    for j in range(1,l1+1):
        qd = score[i-1][j-1] + \
                BLOSUM62[am.index(s2[i-1])][am.index(s1[j-1])]
        qu = score[i-1][j] + g
        ql = score[i][j-1] + g
        sc = [qd,qu,ql]
        tr = ['d','u','l']
        m = max(sc)
        score[i].append(m)
        trace[i].append(tr[sc.index(m)])

a1 = []
a2 = []

# Trace back
i = l2
j = l1
similarity = 0
while(trace[i][j] != '0'):
    if( trace[i][j] == 'd' ):
        a1.append(s1[j-1])
        a2.append(s2[i-1])
        i -= 1
        j -= 1
        similarity += 1
    elif( trace[i][j] == 'l' ):
        a1.append(s1[j-1])
        a2.append('-')
        j -= 1
    else:
        a2.append(s2[i-1])
        a1.append('-')
        i -= 1

a1.reverse()
a2.reverse()

# Printing aligned strings
j = 0
print('Needleman-Wunsch Similarity ===> ', \
        round( 100*similarity/len(a1), 2), ' %\n')
print('=> ', sys.argv[1], ' aligned')
for i in a1:
    print(i, end='')
    j += 1
    if( j == 60 ):
        j = 0
        print()
print('\n')

print('=> ', sys.argv[2], ' aligned')
for i in a2:
    print(i, end='')
    j += 1
    if( j == 60 ):
        j = 0
        print()
print()
