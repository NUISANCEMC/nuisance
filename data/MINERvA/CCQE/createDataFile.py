from ROOT import *
import sys

def getDecomp( hist):
    nBins = hist.GetNbinsX()
    Mat = TMatrixD(nBins,nBins)

    for i in range(nBins):
            for j in range(nBins):
                    Mat[i][j] = hist.GetBinContent(i+1,j+1)

    tempMat = Mat.Clone()
    testMat = Mat.Clone()

    decompMat = TDecompSVD(tempMat)
    decompMat.Decompose()

    V = decompMat.GetV()
    Sig = decompMat.GetSig()

    S = TMatrixD(nBins,nBins)

    Vt = V.Clone()
    Vt = Vt.Transpose(Vt)

    for i in range(nBins):
        S[i][i] = sqrt(Sig[i])

    J = TMatrixD(nBins,nBins)
    J = V * S * Vt

    newhist = hist.Clone()

    for i in range(nBins):
            for j in range(nBins):
                    newhist.SetBinContent(i+1,j+1,J[i][j])

    return newhist

def getInvert(hist):

        mat = TMatrixD(hist.GetNbinsX(),hist.GetNbinsX())

        for i in range(hist.GetNbinsX()):
            for j in range(hist.GetNbinsX()):
                mat[i][j] = hist.GetBinContent(i+1,j+1)

                invmat = mat.Clone()

        invmat.SetTol(1.6E-20)
        decomp = TDecompSVD(invmat)
        decomp.SetTol(1.6E-20)

        decomp.Decompose()
        decomp.Invert(invmat)
        invhist = hist.Clone(hist.GetName()+"_invert")
        for i in range(hist.GetNbinsX()):
            for j in range(hist.GetNbinsX()):
                invhist.SetBinContent(i+1,j+1,invmat[i][j])

        ident = hist.Clone(hist.GetName()+"_ident")
        for i in range(hist.GetNbinsX()):
            for j in range(hist.GetNbinsX()):
                val_ij = 0.0
                for k in range(hist.GetNbinsX()):
                    val_ij += invmat[i][k]*mat[k][j]
                ident.SetBinContent(i+1,j+1,val_ij)

#       hist.Scale(1E-76)                                                                                                                                                                                   
#       invhist.Scale(1E76)                                                                                                                                                                                 
        return invhist, ident


measurementName = str(sys.argv[1])
dataFile = str(sys.argv[2])
covFile = str(sys.argv[3])
dataFile_shp = str(sys.argv[4])
covFile_shp  = str(sys.argv[5])

print measurementName
print dataFile
print covFile

plotTitles = ";Q2;Cross;"
covTitles = ";Q2;Q2;"

gr = TGraphErrors(dataFile,"%lg %lg %lg")

xBins = gr.GetX()
yBins = gr.GetY()
yErr  = gr.GetEY()
nBins  = gr.GetN()

print "xbins = ",xBins
print "yBins = ",yBins
print "yErr = ",yErr
print "nBins = ",nBins

dataHist = TH1D("data",measurementName+plotTitles, nBins-1, xBins)
correl = TH2D("fullcrl",measurementName+covTitles+";Correlation",nBins-1,xBins,nBins-1,xBins)
correl_diag = TH2D("diagcrl",measurementName+covTitles+";Correlation",nBins-1,xBins,nBins-1,xBins)
covar = TH2D("fullcov",measurementName+covTitles+";Covariance",nBins-1,xBins,nBins-1,xBins)
covar_diag = TH2D("diagcov",measurementName+covTitles+";Covariance",nBins-1,xBins,nBins-1,xBins)

with open(covFile,"r") as f:
    row = 0
    for line in f:
        print yBins[row],yErr[row]

        dataHist.SetBinContent(row+1,yBins[row])
        dataHist.SetBinError(row+1,yErr[row])

        for column in range(nBins-1):
            covar.SetBinContent(row+1,column+1, float((line.split())[column]) *yErr[row]*yErr[column]*1E76)
            correl.SetBinContent(row+1,column+1,float((line.split())[column]))
            
            covar_diag.SetBinContent(row+1,column+1, (row==column)*float((line.split())[column]) *yErr[row]*yErr[column]*1E76)
            correl_diag.SetBinContent(row+1,column+1,(row==column)*float((line.split())[column]))

            
        row += 1
        if row == nBins -1:
            break

outfile = TFile(measurementName,"RECREATE")
outfile.cd()
dataHist.Write()
correl.Write()
covar.Write()
correl_diag.Write()
covar_diag.Write()
inv = getInvert(covar)
inv[0].Write("fullcovinv")
inv[1].Write("fullcovidt")

inv = getInvert(covar_diag)
inv[0].Write("diagcovinv")
inv[1].Write("diagcovidt")

getDecomp(covar).Write("fullcovdec")
getDecomp(covar_diag).Write("diagcovdec")

plotTitles = ";Q2;Cross;"
covTitles = ";Q2;Q2;"

gr = TGraphErrors(dataFile_shp,"%lg %lg %lg")

xBins = gr.GetX()
yBins = gr.GetY()
yErr  = gr.GetEY()
nBins = gr.GetN()

print xBins
print yBins
print yErr
print nBins

shp_dataHist = TH1D("shp_data",measurementName+plotTitles, nBins-1, xBins)
shp_correl = TH2D("shp_fullcrl",measurementName+covTitles+";Correlation",nBins-1,xBins,nBins-1,xBins)
shp_correl_diag = TH2D("shp_diagcrl",measurementName+covTitles+";Correlation",nBins-1,xBins,nBins-1,xBins)
shp_covar = TH2D("shp_fullcov",measurementName+covTitles+";Covariance",nBins-1,xBins,nBins-1,xBins)
shp_covar_diag = TH2D("shp_diagcov",measurementName+covTitles+";Covariance",nBins-1,xBins,nBins-1,xBins)

print "REading shape form ",covFile_shp
with open(covFile_shp,"r") as f:
    row = 0
    for line in f:
        print yBins[row],yErr[row]

        shp_dataHist.SetBinContent(row+1,yBins[row])
        shp_dataHist.SetBinError(row+1,yErr[row])

        for column in range(nBins-1):
            shp_covar.SetBinContent(row+1,column+1, float((line.split())[column]) *yErr[row]*yErr[column]*1E76)
            shp_correl.SetBinContent(row+1,column+1,float((line.split())[column]))

            shp_covar_diag.SetBinContent(row+1,column+1, (row==column)*float((line.split())[column]) *yErr[row]*yErr[column]*1E76)
            shp_correl_diag.SetBinContent(row+1,column+1,(row==column)*float((line.split())[column]))

            
        row += 1

        if row == nBins-1:
            break

shp_dataHist.Write()
shp_correl.Write()
shp_covar.Write()
shp_correl_diag.Write()
shp_covar_diag.Write()

getDecomp(shp_covar).Write("shp_fullcovdec")
getDecomp(shp_covar_diag).Write("shp_diagcovdec")


inv = getInvert(shp_covar)
inv[0].Write("shp_fullcovinv")
inv[1].Write("shp_fullcovidt")

inv = getInvert(shp_covar_diag)
inv[0].Write("shp_diagcovinv")
inv[1].Write("shp_diagcovidt")
outfile.Close()
