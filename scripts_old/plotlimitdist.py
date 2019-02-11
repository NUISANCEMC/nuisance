from ROOT import *
from array import array
import sys

def GetUID(name): 
    return name.replace("_" + name.split("_")[-1],"")
def GetValue(name): 
    return float(name.split("_")[-1])


colorlist = [kGreen,kRed, kBlue, kOrange]

infile = TFile(sys.argv[1], "READ")
alllimitdir = infile.Get("Limits")

# Make starting legend
leg = TLegend(0.1,0.1,0.9,0.9)
alluidstyles = {}
alllines = []

c1 = TCanvas("c1","c1",1000,600)

for dirkey in alllimitdir.GetListOfKeys():
    if dirkey.GetName() == "nominal":
        linedata = TLine(0.1,0.1,0.9,0.9)
        linedata.SetLineColor(kBlack)
        leg.AddEntry(linedata, "DATA", "l")
        alllines.append(linedata)

        linemc = TLine(0.1,0.1,0.9,0.9)
        linemc.SetLineColor(kGreen)
        leg.AddEntry(linemc, "Nominal MC", "l")
        alllines.append(linemc)
        alluidstyles["nominal"] = kGreen

    else:
        uid = GetUID(dirkey.GetName())
        if uid not in alluidstyles:
            alluidstyles[uid] = colorlist[len(alluidstyles)]
            line = TLine(0.1,0.1,0.9,0.9)
            line.SetLineColor( alluidstyles[uid] )
            leg.AddEntry(line, uid, "l")
            alllines.append(line)

leg.Draw()
gPad.Update()
c1.SaveAs("limitplots.pdf(")
gStyle.SetOptTitle(1)
c1.Clear()
c1.Divide(3,1)

# Get Nominal
nomdir = infile.Get("Limits/nominal")
for plotkey in nomdir.GetListOfKeys():

    # Only get data
    if (not plotkey.GetName().endswith("_data")): continue

    # Get Plots
    data = nomdir.Get(plotkey.GetName())
    datashape = nomdir.Get(plotkey.GetName()).Clone()
    mc   = nomdir.Get(plotkey.GetName().replace("data","MC"))
    mcshape   = nomdir.Get(plotkey.GetName().replace("data","MC_SHAPE"))

    if not data or not mc: continue

    # Draw nominal and data
    c1.cd(1)
    data.GetYaxis().SetRangeUser(0.0, data.GetMaximum()*1.5)
    data.SetLineColor(kBlack)
    data.SetLineWidth(3)
    data.Draw("E1")
    mc.SetLineColor(kGreen)
    mc.SetLineWidth(3)
    mc.Draw("SAME HIST")
    alllimitmc = []

    c1.cd(2)
    datashape.GetYaxis().SetRangeUser(0.0, data.GetMaximum()*1.5)
    datashape.SetLineColor(kBlack)
    datashape.SetLineWidth(3)
    datashape.SetTitle(datashape.GetName() + "_SHAPE")
    datashape.Draw("E1")
    mcshape.SetLineWidth(3)
    mcshape.SetLineColor(kGreen)
    mcshape.Draw("SAME HIST")

    likelihoodvals = {}
    maxlike = 0.0
    minlike = 1.E9



    # Loop through and get limit plots
    for dirkey in alllimitdir.GetListOfKeys():
        if dirkey.GetName() == "nominal": continue
        limitdir = alllimitdir.Get(dirkey.GetName())              
        limitmc = limitdir.Get(plotkey.GetName().replace("_data","_MC")).Clone()
        limitmcshape = limitdir.Get(plotkey.GetName().replace("_data","_MC_SHAPE")).Clone()
        
        print "Getting from key:", limitdir, limitmc
        uid = GetUID(dirkey.GetName())
        if uid not in likelihoodvals:
            likelihoodvals[uid] = []

        limitmc.SetLineColor( alluidstyles[uid] )
        limitmc.SetLineWidth(1)

        limitmcshape.SetLineColor(  alluidstyles[uid] )
        limitmcshape.SetLineWidth(1)

        c1.cd(1)
        limitmc.Draw("SAME HIST C")
        alllimitmc.append(limitmc) 

        c1.cd(2)
        limitmcshape.Draw("SAME HIST C")
        alllimitmc.append(limitmcshape)

        xval = float(GetValue(dirkey.GetName()))
        yval = float(limitmc.GetTitle())
        likelihoodvals[uid].append([xval,yval])

        maxlike = max([maxlike, (float(limitmc.GetTitle()))])
        minlike = min([minlike, (float(limitmc.GetTitle()))])


    
    #likelihoodvals["nominal"] = [[-1000, minlike],
#                                 [1000, minlike]]
#    alluidstyles["nominal"] = kGreen
#    likelihoodvals["nominal2"] = [[1000, minlike+1.00],
#                                 [-1000, minlike+1.00]]
#    alluidstyles["nominal2"] = kGreen
#    likelihoodvals["nominal3"] = [[-1000, minlike+7.82],
#                                 [1000, minlike+7.82]]
#    alluidstyles["nominal3"] = kGreen
    
    # Make like hist
    c1.cd(3)
    dif = (maxlike - minlike)/10.0
    allgr = []
    for i, uid in enumerate(sorted(likelihoodvals)):
        print i, uid
        xvals = []
        yvals = []
        for valset in sorted(likelihoodvals[uid]):
            xvals.append(valset[0])
            yvals.append(valset[1])

 
        gr = TGraph(len(xvals), array('f',xvals), array('f',yvals))
        gr.SetTitle("Likelihood Scan about Nominal")
        gr.SetLineColor(  alluidstyles[uid] )

        if  "nominal" in uid:
            gr.SetMarkerStyle(20)
            gr.SetMarkerColor(kGreen)
        if i == 0:
            gr.Draw("APL")
        else: gr.Draw("SAME PL")

        gr.GetYaxis().SetRangeUser(minlike - dif, maxlike + dif)
        allgr.append(gr)

    c1.Update()
    c1.SaveAs("limitplots.pdf")

c1.SaveAs("limitplots.pdf)")



        
        




