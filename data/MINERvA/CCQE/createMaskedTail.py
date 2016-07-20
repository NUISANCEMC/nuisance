import sys

maskedBins = map(int,sys.argv[3].split())

print "Reading from ", sys.argv[1]," and ", sys.argv[2]

with open(sys.argv[1],"r") as infile:

    mskNum = len(maskedBins)

    name = sys.argv[1].strip(".txt") + "_msk_" + str(mskNum) +".txt"
    print name

    with open(name,"w") as outfile:

        #length  = len(infile.readlines())
        length = 0
        for row, line in enumerate(infile):

            nums = line.split()

            print row, length,mskNum, length-1-mskNum

            if row == length-1-mskNum:
                outfile.write(nums[0] + ' 0.0   0.0   \n')           
            
            elif row in maskedBins:
                continue


            else:
                for val in nums:
                    outfile.write(val + '     ')
                outfile.write('  \n')
            


with open(sys.argv[2],"r") as infile:

    name = sys.argv[2].strip(".txt") + "_msk_" + str(mskNum) +".txt"

    with open(name, "w") as outfile:

        for row, line in enumerate(infile):
            
            nums = line.split()
            
            for column, val in enumerate(nums):
                
                if column in maskedBins or row in maskedBins:
                    continue
                else:
                    print val + " ",
                    outfile.write(val + ' ')

            print ""
            outfile.write(' \n')
