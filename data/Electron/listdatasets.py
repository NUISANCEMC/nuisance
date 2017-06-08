import sys

uniquelist = []
for line in (open(sys.argv[1])):

    entries = line.strip().split()
    z = entries[0]
    a = entries[1]
    e = entries[2]
    t = entries[3]
    s = entries[7]


    uid = "ElectronData_" + z + "_" + a + "_" + e + "_" + t + "_" + s
    if (uid not in uniquelist):
        uniquelist.append(uid)
        print sys.argv[2] + uid + sys.argv[3]



