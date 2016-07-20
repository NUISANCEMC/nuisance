import sys
infile = open(sys.argv[1],"r")
outfile = open(sys.argv[2],"w")
nbins = eval(sys.argv[3])

myarray = nbins*[nbins*[0.0]]

row = -1
for line in infile:
    if row == -1: 
        row +=1
        continue


    line.replace("$","")
#    print line.split(",")[1:]
    
    col = 0
    for obj in line.split(",")[1:]:

        if (obj) == '': 
            col += 1
            continue

        print col, row, 
        number = float(obj.replace("$",""))
        print number
        myarray[row][col] = number
        myarray[col][row] = number
        col+=1
#        print number
        
#    numbers = map(float,line.split()[3:])
#    print numbers

    row+=1


for i in range(nbins):
    for j in range(nbins):

        outfile.write(str(myarray[j][i])+" ")
    outfile.write("\n")
