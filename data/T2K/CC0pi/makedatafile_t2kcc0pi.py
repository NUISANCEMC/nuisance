from ROOT import *
from array import *
from math import *

def GetMiddle(mystr):

    lims = mystr.strip().split(" - ")
    val = (float(lims[0]) + float(lims[1]))/2.0

    return val

def GetLowEdge(mystr):

    lims = mystr.strip().split(" - ")
    val = (float(lims[0]) + 0.00001)

    return val

def GetHighEdge(mystr):

    lims = mystr.strip().split(" - ")
    val = (float(lims[1]) - 0.00001)

    return val

def GetIndex(mystr):

    lims = mystr.split("-")

    return int(lims[0]), int(lims[1])

outfile = TFile("T2K_CC0PI_2DPmuCosmu_Data.root","RECREATE")

# ANALYSIS I
#______________________________
xedge   =  [0.0, 0.3, 0.4, 0.5, 0.65, 0.8, 0.95, 1.1, 1.25, 1.5, 2.0, 3.0, 5.0, 30.0]
yedge   =  [-1.0, 0.0, 0.6, 0.7, 0.8, 0.85, 0.9, 0.94, 0.98, 1.0]

datahist = TH2D("analysis1_data","analysis1_data",
                len(xedge)-1, array('f',xedge),
                len(yedge)-1, array('f',yedge))

maphist = datahist.Clone("analysis1_map")
maphist.SetTitle("analysis1_map")

counthist = datahist.Clone("analysis1_entrycount")

datapoly = TH2Poly("datapoly","datapoly", 0.0,30.0, -1.0, 1.0)
hist = None

binedges = []
histedgeslist = []
xsecvals = []
histxseclist = []

binlimits = [3,8,15,22,30,39,47,58,67]
with open("cross-section_analysisI.txt") as f:
    count = 0
    for line in f:
        count += 1

        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        ibin = int(   data[0]  ) + 1

        xval = round(float(GetLowEdge( data[2] )),4)
        yval = round(float(GetLowEdge( data[1] )),4)
        xhig = round(float(GetHighEdge( data[2] )),4)
        yhig = round(float(GetHighEdge( data[1] )),4)

        xsec = float( data[3]  ) * 1E-38

        datapoly.AddBin( xval, yval, xhig, yhig )
        datapoly.SetBinContent( datapoly.GetNumberOfBins(), xsec)

        binedges.append( xval )
        xsecvals.append( xsec )
        if ibin in binlimits:
            binedges.append( xhig )
            histedgeslist.append(binedges)
            histxseclist.append(xsecvals)
            binedges = []
            xsecvals = []


        datahist.Fill(xval, yval, xsec)
        counthist.Fill(xval, yval, 1.0)

        for i in range(maphist.GetNbinsX()):
            for j in range(maphist.GetNbinsY()):
                xcent = maphist.GetXaxis().GetBinCenter(i+1)
                ycent = maphist.GetYaxis().GetBinCenter(j+1)

                if (xcent > xval and xcent < xhig and
                    ycent > yval and ycent < yhig):
                    maphist.SetBinContent(i+1,j+1, ibin)


# Get Covariances (keep in 1E-38 cm^2)                 \
nbins = 67

statcov = TH2D("analysis1_statcov","analysis1_statcov", nbins, 0.0, float(nbins), nbins, 0.0, float(nbins));
systcov = TH2D("analysis1_systcov","analysis1_systcov", nbins, 0.0, float(nbins), nbins, 0.0, float(nbins));
normcov = TH2D("analysis1_normcov","analysis1_normcov", nbins, 0.0, float(nbins), nbins, 0.0, float(nbins));
totcov = TH2D("analysis1_totcov","analysis1_totcov", nbins, 0.0, float(nbins), nbins, 0.0, float(nbins));

with open("covariance_statisticUncertainty_analysisI.txt") as f:
    count = 0
    for line in f:
        count += 1

        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        xi, yi = GetIndex(data[0])
        cov    = float(data[1])

        statcov.SetBinContent(xi + 1, yi + 1, cov)

with open("covariance_shapeSystematics_analysisI.txt") as f:
    count = 0
    for line in f:
        count += 1

        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        xi, yi = GetIndex(data[0])
        cov    = float(data[1])

        systcov.SetBinContent(xi + 1, yi + 1, cov)

with open("covariance_fluxNormalizationSystematics_analysisI.txt") as f:
    count = 0
    for line in f:
        count += 1

        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        xi, yi = GetIndex(data[0])
        cov    = float(data[1])

        normcov.SetBinContent(xi + 1, yi + 1, cov)

totcov.Add(systcov)
totcov.Add(statcov)
totcov.Add(normcov)


data1D = TH1D("datahist","datahist", datapoly.GetNumberOfBins(), 0.0, float(datapoly.GetNumberOfBins()));
for i in range(datapoly.GetNumberOfBins()):
    data1D.SetBinContent(i+1, datapoly.GetBinContent(i+1));
    data1D.SetBinError(i+1, sqrt(totcov.GetBinContent(i+1,i+1))*1E-38)

outfile.cd()

for i, obj in enumerate(histedgeslist):
    print obj

    hist = TH1D("dataslice_" + str(i), "dataslice_" + str(i), len(obj)-1, array('f',obj))
    for j in range(hist.GetNbinsX()):
        hist.SetBinContent(j+1, histxseclist[i][j])

    hist.GetXaxis().SetRangeUser(obj[0], obj[len(obj)-1])
    hist.Draw("HIST")
    gPad.Update()

    hist.SetNameTitle("dataslice_" + str(i),"dataslice_" + str(i))
    hist.Write()


outfile.cd()
datahist.Write()
counthist.Write()
maphist.Write()
datapoly.Write()
data1D.Write()
statcov.Write()
systcov.Write()
totcov.Write()
normcov.Write()




# ANALYSIS II
#______________________________
xedge   =  [0.2, 0.35, 0.5, 0.65, 0.8, 0.95, 1.1, 1.25, 1.5, 2.0, 3.0, 5.0, 30.0]
yedge   =  [0.6, 0.7, 0.8, 0.85, 0.9, 0.925, 0.95, 0.975, 1.0]

datahist = TH2D("analysis2_data","analysis2_data",
                len(xedge)-1, array('f',xedge),
                len(yedge)-1, array('f',yedge))

maphist = datahist.Clone("analysis2_map")
maphist.SetTitle("analysis2_map")

counthist = datahist.Clone("analysis2_entrycount")

# Get Data Entries
entries = []
count = 0
with open("rps_crossSection_analysis2.txt") as f:
    for line in f:
        count += 1

        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        ibin = int(   data[0]  ) + 1
        xval = GetMiddle( data[2] )
        yval = GetMiddle( data[1] )
        xsec = float( data[3]  ) * 1E-38

        datahist.Fill(xval, yval, xsec)
        maphist.Fill(xval, yval, ibin)

        counthist.Fill(xval, yval, 1.0)

     #   print ibin, "Map Value"

# Get N Bins
nbins = int(maphist.GetMaximum())
print "NBins I = ", nbins

# Get Covariances (keep in 1E-38 cm^2)
statcov = TH2D("analysis2_statcov","analysis2_statcov", nbins, 0.0, float(nbins), nbins, 0.0, float(nbins));
systcov = TH2D("analysis2_systcov","analysis2_systcov", nbins, 0.0, float(nbins), nbins, 0.0, float(nbins));
normcov = TH2D("analysis2_normcov","analysis2_normcov", nbins, 0.0, float(nbins), nbins, 0.0, float(nbins));
totcov = TH2D("analysis2_totcov","analysis2_totcov", nbins, 0.0, float(nbins), nbins, 0.0, float(nbins));

with open("rps_statsCov_analysis2.txt") as f:
    count = 0
    for line in f:
        count += 1

        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        xi, yi = GetIndex(data[0])
        cov    = float(data[1])

        statcov.SetBinContent(xi + 1, yi + 1, cov)

with open("rps_systCov_analysis2.txt") as f:
    count = 0
    for line in f:
        count += 1

        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        xi, yi = GetIndex(data[0])
        cov    = float(data[1])

        systcov.SetBinContent(xi + 1, yi + 1, cov)

with open("rps_fluxNormCov_analysis2.txt") as f:
    count = 0
    for line in f:
        count += 1

        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        xi, yi = GetIndex(data[0])
        cov    = float(data[1])

        normcov.SetBinContent(xi + 1, yi + 1, cov)

totcov.Add(systcov)
totcov.Add(statcov)
totcov.Add(normcov)

outfile.cd()
datahist.Write()
maphist.Write()
counthist.Write()
statcov.Write()
systcov.Write()
totcov.Write()
normcov.Write()




#  LocalWords:  xval
