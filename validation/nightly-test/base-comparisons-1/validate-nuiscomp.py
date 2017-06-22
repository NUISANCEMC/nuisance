from ROOT import *
import sys

print "Reading Infile_NEW :",sys.argv[1]
infile_new   = TFile(sys.argv[1],"READ")
print "Reading infile bench :",sys.argv[2]
infile_bench = TFile(sys.argv[2],"READ")
print "Reading outfile :",sys.argv[3]
outfile      = sys.argv[3]

c1 = TCanvas("c1","c1",900,300)
c1.Divide(3,1)


totalcount = 0
errorcount = 0

keylist = infile_new.GetListOfKeys()
# Get N Valid Keys
nkeys = 0
for obj in keylist:
    hist_new   = infile_new.Get(obj.GetName())
    if "TH1D" in str(type(hist_new)):
        nkeys += 1


for obj in keylist:

    hist_new   = infile_new.Get(obj.GetName())
    hist_bench = infile_bench.Get(obj.GetName())

    if hist_new and not hist_bench:
        print "ERROR: Cannot find bench mark plot for : ",obj.GetName()
        hist_bench = hist_new.Clone("newbench")
        hist_bench.Reset()
        
    if "TH1D" in str(type(hist_new)):

        hist_dif = hist_new.Clone("dif")
        hist_dif.Add(hist_bench,-1.0)

        hist_bench.SetLineColor(kBlack)
        hist_bench.SetLineWidth(3)
        hist_new.SetLineColor(kGreen)
        hist_dif.SetLineColor(kRed)
        gStyle.SetOptTitle(1)

        c1.cd(1)
        hist_bench.SetTitle(obj.GetName())
        hist_bench.Draw()
        hist_new.Draw("SAME")

        c1.cd(2)
        hist_new.SetTitle("Latest")
        hist_new.Draw()

        c1.cd(3)
        hist_dif.SetTitle("Difference : " + str(hist_dif.Integral()))
        hist_dif.Draw()

        c1.Update()

        tolerance = 1E-4
        if (fabs(hist_dif.Integral()) > tolerance):
            print "ERROR: Difference found for: ", obj.GetName()

            errorsave = outfile + "_errors.pdf"
            if errorcount == 0: errorsave += "("
            c1.SaveAs(errorsave)
            errorcount+=1
            
        totalsave = outfile + "_total.pdf"
        if totalcount == 0: totalsave += "("

        c1.SaveAs(totalsave)
        totalcount+=1


leg = TLegend(0.1,0.1,0.9,0.9)
blank = TLine(0.0,0.0,1.0,1.0)
leg.AddEntry(blank,"End of Plots","")
c1.cd()
leg.Draw()
if totalcount > 0: c1.SaveAs(totalsave + ")")
if errorcount > 0: c1.SaveAs(errorsave + ")")


