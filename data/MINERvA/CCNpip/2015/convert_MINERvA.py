#!/usr/bin/python

import sys

if len(sys.argv) == 1:
  print "I need one argument: the filename"
  sys.exit(-1)

filename = sys.argv[1]

if filename.find("conv.csv") != -1:
  print "The input file already has conv.csv ending"
  print "I'm assuming this is already converted, so exiting"
  print "BYE!"
  sys.exit()

# Open the input file as read only
f = open(filename, 'r')

# Read first line
array = []
length = 0
for line in f:
  # Find the longest length of a line; this is the dimension of the covariance
  if len(line.split()) > length:
    length = len(line.split())
  # Write the input to a data structure
  array.append([x for x in line.split()])
f.close()

print "Maximum length found: ", length


# Save the old file as a backup because we're going to over-write the old one!
backupname = filename+'.bak'
f = open(backupname, 'w')
for i in array:
  for j in i:
    f.write(j)
    f.write(' ')
  f.write('\n')
f.close()




newmat = []
cnter = 0

for i in range(length):
  tempmat = []
  for j in range(length):
#   When we need to make i entry = j entry
    if (length - j > len(array[i])):
      cnter+=1
      tempmat.append(newmat[j][i])
#   Otherwise we want to pick up the next element
    else:
      tempmat.append(array[i][j-i])

# print tempmat
  newmat.append(tempmat)

if cnter == 0:
  print "I don't think you wanted to convert this because it already looks like a valid correlation/covariance matrix!"
  print "So I'm exiting now! Bye!"
  sys.exit()

# Replace the output name
output = filename
fout = open(output, 'w')

# Print the output
for i in newmat:
  for k in i:
    fout.write(k)
    fout.write(' ')
  fout.write('\n')

fout.close()
