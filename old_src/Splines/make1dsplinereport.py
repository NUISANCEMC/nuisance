from ROOT import *
import sys


c1 = TCanvas("c1","c1",900,300)
c1.Divide(3,1)

infile = TFile(sys.argv[1],"READ")
maxn = len(infile.GetListOfKeys())
print maxn

for i, key in enumerate(infile.GetListOfKeys()):
    if ("Paramater_Set" not in key.GetName()): continue

    gStyle.SetOptTitle(1)

    c1.cd(1)
    parset = infile.Get(key.GetName())
    parset.SetTitle(key.GetName().replace("_"," "))
    parset.Draw()

    c1.cd(2)
    weightscan.Draw(key.GetName().replace("Paramater","RawWeights") + " >> temphist")
    weighthist = temphist.Clone("weighthist")
    temphist.Delete()
    del temphist

    weightscan.Draw(key.GetName().replace("Paramater","SplineWeights") + " >> temphist")
    splinehist = temphist.Clone("splinehist")
    temphist.Delete()
    del temphist

    splinehist.SetLineColor(kRed)
    weighthist.SetTitle("Raw Weights")
    splinehist.SetTitle("Spline Weights")
    weighthist.Draw()
    splinehist.Draw("SAME")
    gPad.BuildLegend()
    weighthist.SetTitle("Event Weights")
    gPad.SetLogy(1)
    gPad.Update()

    c1.cd(3)
    weightscan.Draw(key.GetName().replace("Paramater","DifWeights") + " >> temphist")
    difhist = temphist.Clone("difhist")
    temphist.Delete()
    del temphist
    
    difhist.SetTitle("Difference")
    difhist.Draw("HIST")
    gPad.SetLogy(1)
    gPad.Update()

    if (i == 0): savestring = (sys.argv[1].replace(".root.splinetest.1DEventScan.root","_splinetest_1DEventScan.pdf("))
    elif (i == (maxn)-2): savestring = (sys.argv[1].replace(".root.splinetest.1DEventScan.root","_splinetest_1DEventScan.pdf)"))
    else: savestring = (sys.argv[1].replace(".root.splinetest.1DEventScan.root","_splinetest_1DEventScan.pdf"))

    c1.SaveAs(savestring)
