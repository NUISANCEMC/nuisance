from ROOT import TFile, TFolder, TObjString
import sys

def PrintKey(obj, indent):
    splitkey = obj.GetName().split(";")
    keydict  = {}
    for val in reversed(splitkey):
        splitval = val.split(":")
        if (len(splitval) < 2): splitval = val.split("=")
        keydict[splitval[0].strip()] = ''.join(splitval[1:]).strip()
        
    print indent, keydict

def ExpandKeys(keydir, indent):
    for obj in keydir.GetListOfFolders():
        if "TFolder" in str(type(obj)):
            print indent, obj.GetName()
            ExpandKeys(obj, indent + " -> ")
        if "TObjString" in str(type(obj)):
            PrintKey(obj, indent )

def ExpandGlobalList(keydir):
    for obj in keydir.GetListOfFolders():
        if str(obj.GetName()) != "GlobalParameterList": continue
        print "GLOBAL : ", obj.GetName()
        ExpandKeys(obj,"GLOBAL : ")

if __name__=="__main__":

    myfile = TFile(sys.argv[1],"READ")
    configs = myfile.Get("gconfig")
    ExpandKeys(configs,"")

    print "\n# Global List #\n"
    ExpandGlobalList(configs)
