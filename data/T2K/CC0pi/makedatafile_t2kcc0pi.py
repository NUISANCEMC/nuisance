from ROOT import *
from array import *

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
           
with open("cross-section_analysisI.txt") as f:
    count = 0
    for line in f:
        count += 1
        
        if (count < 4): continue
        data = line.strip().split("|")
        if (len(data) < 1): continue

        ibin = int(   data[0]  ) + 1
        
        xval = GetLowEdge( data[2] )
        yval = GetLowEdge( data[1] )
        xhig = GetHighEdge( data[2] )
        yhig = GetHighEdge( data[1] )
        
        xsec = float( data[3]  ) * 1E-38

        datahist.Fill(xval, yval, xsec)
        counthist.Fill(xval, yval, 1.0)

        for i in range(maphist.GetNbinsX()):
            for j in range(maphist.GetNbinsY()):
                xcent = maphist.GetXaxis().GetBinCenter(i+1)
                ycent = maphist.GetYaxis().GetBinCenter(j+1)

                if (xcent > xval and xcent < xhig and
                    ycent > yval and ycent < yhig):
                    maphist.SetBinContent(i+1,j+1, ibin)

outfile.cd()
datahist.Write()
counthist.Write()
maphist.Write()


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

        print ibin, "Map Value"
        
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

        statcov.SetBinContent(xi, yi, cov)

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



