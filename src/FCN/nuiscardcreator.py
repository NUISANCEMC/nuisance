import argparse
from ROOT import *
import os, sys
import xml.etree.ElementTree as ET
from subprocess import call
import inspect

def PrintCard():

   inputlist = {}
   inputtype = {}

   i = ET.parse('mysample.xml')
   for itype in i.findall('input'):

      filetype = itype.get('type')
      filename = itype.get('file')

      if filetype and filename:
         filegen  = filename.split(":")[0]
         filename = filename.split(":")[1]

         if not filename: continue

         inputlist[filetype] = filename
         inputtype[filetype] = filegen

   e = ET.parse('sample_list.xml')
   for atype in e.findall('input'):


      samplename = atype.get('sample')
      sampletype = atype.get('type')
      newgen = ""

      typesplit = sampletype.replace(",",";").split(";")
      oldf = len(typesplit)
      newf = 0
      for obj in typesplit:
         newobj = obj.replace("TYPE:","").replace("(","").replace(")","")

         if newobj in inputlist:
            sampletype = sampletype.replace(newobj, inputlist[newobj])
            sampletype = sampletype.replace("TYPE", inputtype[newobj])

            newf += 1
      if newf != oldf: continue


def PrintTypes():

   print "<nuisance>"
   objlist = []
   e = ET.parse('sample_list.xml')
   for atype in e.findall('input'):
      sampletype = atype.get('type')
      sampletype = sampletype.replace("TYPE:","").replace("(","").replace(")","")
      typesplit = sampletype.replace(",",";").split(";")

      for obj in typesplit:
         if obj not in objlist:
            objlist.append(obj)
            print '< type="'+obj+'"  file="TYPE:" />'
   print "</nuisance>"

forceful  = False
xmlformat = True
def CreateBlankInputXML(outputcard):

   # Parse Sample List
   samplelist    = str(os.environ['NUISANCE']) + '/src/FCN/sample_list.xml'
   samplelistxml = ET.parse(samplelist)

    # Keep track of unique ids
   idlist = []
   commentlist = []

   # Loop over all samples and find unique ids
   for sample in samplelistxml.findall('input'):

      # Read Comments
      comment = sample.get('eventcomment')
      if (comment):
         commentlist.append(comment)
         continue

      # Read Inputs
      inputid = sample.get('type')
      if (inputid):
         inputid = inputid.replace("TYPE:","").replace("(","").replace(")","")

         # Get all unique ids after split
         inputsplit = inputid.replace(",",";").split(";")
         for id in inputsplit:
            if id not in idlist:
               idlist.append(id)

   # Check for output card
   if os.path.isfile(outputcard) and not forceful:
      print "File:", outputcard, "already exists. Use -f flag to overwrite."
      sys.exit(-1)

   # Save out unique ids to xml (by writing manually...)
   f = open(outputcard,"w")

   f.write("<inputs>\n\n")

   f.write('  <!-- INPUT NOTES --> \n')
   for comment in commentlist:
      f.write('  <!-- ' + comment + ' --> \n')
   f.write('\n\n')

   for id in idlist:
      f.write('  <input type="' + id + '"  file="TYPE:/path/to/' + id + '_file.root" /> \n')
   f.write("</inputs>")

   # Print out useage
   print "Created a new blank input example inside :", outputcard
   print "Fill in all the data files you care about, and then run: \n"
   print " $ nuiscardcreator -inputs " + outputcard + " -output nuisance.card \n"
   print "to generate a working nuisance card file. "

   sys.exit(0)

def CheckComment(checks, comparison):

   for check in checks.split("/"):
      found = True
      for obj2 in check.split("+"):
         if obj2 not in comparison: found = False
      if found: return True

   return False

def GenerateCardXML(inputs, outputs):

   inputfiles = {}
   inputtypes = {}

   # Parse our input card first
   inputlistxml = ET.parse(inputs)
   for inputfile in inputlistxml.findall('input'):

      # Get type and file
      filetype = inputfile.get('type')
      filename = inputfile.get('file')

      # Check valid
      if not filetype or not filename: continue

      # Split by path and type
      filegenr, filepath = filename.split(":")

      # Check both valid
      if not filegenr or not filepath: continue

      # Fill our containers
      inputfiles[filetype] = filepath
      inputtypes[filetype] = filegenr

   print inputfiles

   # Parse Sample List
   samplelist    = str(os.environ['NUISANCE']) + '/src/FCN/sample_list.xml'
   samplelistxml = ET.parse(samplelist)

   # container for output lines
   sampleoutputs = []
   allsamples = ''

   # Parser
   for sample in samplelistxml.findall('input'):

      # Allow for actions + comments
      if (sample.get('comment')):

         # Add to list
         sampleoutputs.append( ['@COMMENT', sample.get('comment'), sample.get('check')] )
         continue

      if (sample.get('action')):
         sampleoutputs.append( ['@ACTION', sample.get('action')] )
         continue

      # Get information
      samplename = sample.get('sample')
      sampletype = sample.get('type')


      # Check valid
      if not samplename or not sampletype: continue

      # Seperate into uids
      typesplit = sampletype.replace(",",";").split(";")

      # Make sure we can replace all uids
      startlength = len(typesplit)
      endlength  = 0

      # Loop over all ids
      for uid in typesplit:
         parseduid = uid.replace("TYPE:","").replace("(","").replace(")","")

         # Check we have this file given
         if not parseduid in inputfiles: continue

         # Replace UID in samplelist with file given by user
         sampletype = sampletype.replace(parseduid, inputfiles[parseduid])
         sampletype = sampletype.replace("TYPE",    inputtypes[parseduid])

         # Count how many uids replaced
         endlength += 1

      # If not all uids caught, skip
      if startlength != endlength: continue
      sampleoutputs.append( [samplename, sampletype] )
      allsamples += " " + samplename

   # Setup outputs
   outputfile = outputs
   if not outputfile:
      outputfile = inputs.replace(".xml","") + ".nuisance.xml"

   if os.path.isfile(outputfile) and not forceful:
      print "File:", outputfile, "already exists. Use -f flag to overwrite."
      sys.exit(-1)

   f = open(outputfile,"w")
   f.write("<nuisance>\n")
   for id in sampleoutputs:

      # Actions
      if id[0] == '@ACTION':
         if id[1] == 'BREAK':
            f.write('  <!-- -------------------------- --> \n' )
         if id[1] == 'SKIP':
            f.write('  \n' )
         continue

      # Comments
      if id[0] == '@COMMENT':
         if id[2] and not CheckComment(id[2],allsamples): continue

         f.write( '  <!-- ' + id[1] + ' --> \n')
         continue

      # Main Writing
      f.write('  <sample name="' + id[0] + '"  file="' + id[1] + '" /> \n')
   f.write("</nuisance>")


   sys.exit(0)

if __name__ == '__main__':

   searchdescrip = "Set of tools used to generate nuisance card files"
   parser = argparse.ArgumentParser(description=searchdescrip)
   parser.add_argument('-blankinput' , help='Produce blank input file.')
   parser.add_argument('-blankcard'  , help='Copies src/FCN/sample_list.xml to current directory')
   parser.add_argument('-f'          , help='Force overwrites', action='store_true')
   parser.add_argument('-generate'   , help='Generates a cardfile')
   parser.add_argument('-output'     , help='Output')
   parser.add_argument('-format'     , help='Format')
   args = parser.parse_args()

   if (args.f):
      forceful = True

   if (args.format):
      if args.format == "xml": xmlformat = True
      elif args.format == "txt": xmlformat = False

   if (xmlformat):
      if (args.blankinput):
         CreateBlankInputXML(args.blankinput)

      if (args.generate):
         GenerateCardXML(args.generate, args.output)


