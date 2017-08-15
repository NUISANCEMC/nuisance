from ROOT import *

def ConvertToGENIEDial(name, val, err):
    if name == "MaRES":
        convval = ((val / 1.12) - 1.0) / 0.20
        converr = ((err / 1.12) / 0.20)
        return "MaCCRES", convval, converr
    
    elif name == "DISNorm":
        convval = ((val - 100.0) / 100.0)/0.5
        converr = (err / 100.0)/0.5
        return "NonRESBGvnCC1pi,NonRESBGvpCC1pi,NonRESBGvbarnCC1pi,NonRESBGvbarpCC1pi", convval, converr

    elif name == "RESNorm":
        convval = ((val - 100.0)/100.0)
        converr = err / 100.0
        return "MINERvARW_NormCCRES", convval, converr

def MakePriorROOTFile(name, fit, cor):

    out = TFile("bctuneprior_" + name + ".root","RECREATE")
    out.cd()

    # Make Dial Plot
    dials = TH1D("dials", "dials;Dials;Dial Value",
                 len(fit),0.0,float(len(fit)))
    i = 0
    for result in fit:
        name, val, err = ConvertToGENIEDial( result[0], result[1], result[2] )
        print name, val, err
        dials.GetXaxis().SetBinLabel(i+1, name)
        dials.SetBinContent(i+1, val)
        dials.SetBinError(i+1, err)
        i += 1
    dials.Write()

    # Make Cover Plot
    covars = TH2D("covariance","covariance;Dials;Dials;Covar",
                  len(fit),0.0,float(len(fit)),
                  len(fit),0.0,float(len(fit)))
    for i in range(len(fit)):
        for j in range(len(fit)):
            
            namei, vali, erri = ConvertToGENIEDial( fit[i][0], fit[i][1], fit[i][2] )
            namej, valj, errj = ConvertToGENIEDial( fit[j][0], fit[j][1], fit[j][2] )
            
            covars.GetXaxis().SetBinLabel(i+1,namei)
            covars.GetYaxis().SetBinLabel(j+1,namej)
            covars.SetBinContent(i+1,j+1, erri*errj*cor[i][j])
    covars.Write()

    # Close
    out.Close()
    




# Fit results [val,err]
# Correlation should be in same order
name_res = "WithNormRES"
fit_res = [ ["MaRES",   0.94,0.05],
            ["DISNorm", 46, 4],
            ["RESNorm", 115,7] ]
cor_res = [ [1.0, -0.3, -0.9],
            [-0.3, 1.0, 0.2],
            [-0.9, 0.2, 1.0] ]
MakePriorROOTFile(name_res, fit_res, cor_res)



