import os,ROOT

fname = "MINERvA_TripleDiffQELike_ptpzsumtp.root"
hname = "ptpzsumtp_data_cross_section_with_total_unc"
hcorrname = "TotalCovariance"

f = ROOT.TFile("./dataFromPaper/"+fname)

h_3D = f.Get(hname)
h_3D.SetName(hname)
h_corr = f.Get(hcorrname)

nx = h_3D.GetNbinsX()+2
ny = h_3D.GetNbinsY()+2
nz = h_3D.GetNbinsZ()+2

nfinal = nx*ny*nz

print("@@ Printing X (SumTp)")
for ix in range(0, nx):

  x_l = h_3D.GetXaxis().GetBinLowEdge(ix)
  x_r = h_3D.GetXaxis().GetBinUpEdge(ix)
  x_c = (x_l+x_r)/2.
  print("[%1.2f, %1.2f]"%(x_l,x_r))
print("-> NbinsX = %d"%(h_3D.GetNbinsX()))
print("@@ Printing Y (Pt)")
for iy in range(0, ny):

  y_l = h_3D.GetYaxis().GetBinLowEdge(iy)
  y_r = h_3D.GetYaxis().GetBinUpEdge(iy)
  y_c = (y_l+y_r)/2.
  print("[%1.2f, %1.2f]"%(y_l,y_r))
print("-> NbinsY = %d"%(h_3D.GetNbinsY()))
print("@@ Printing Z (Pz)")
for iz in range(0, nz):

  z_l = h_3D.GetZaxis().GetBinLowEdge(iz)
  z_r = h_3D.GetZaxis().GetBinUpEdge(iz)
  z_c = (z_l+z_r)/2.
  print("[%1.2f, %1.2f]"%(z_l,z_r))
print("-> NbinsZ = %d"%(h_3D.GetNbinsZ()))
print(nx,ny,nz,nfinal)

f_out = ROOT.TFile("data_ptpzsumtp_1D.root","RECREATE")
h_out = ROOT.TH1D(hname+"_1D", "", nfinal, 0., 1.*nfinal)

ibin=0 # increased by hand
for iz in range(0, nz):

  z_l = h_3D.GetZaxis().GetBinLowEdge(iz)
  z_r = h_3D.GetZaxis().GetBinUpEdge(iz)
  z_c = (z_l+z_r)/2.

  for iy in range(0, ny):

    y_l = h_3D.GetYaxis().GetBinLowEdge(iy)
    y_r = h_3D.GetYaxis().GetBinUpEdge(iy)
    y_c = (y_l+y_r)/2.

    for ix in range(0, nx):

      x_l = h_3D.GetXaxis().GetBinLowEdge(ix)
      x_r = h_3D.GetXaxis().GetBinUpEdge(ix)
      x_c = (x_l+x_r)/2.

      binFromCoord = h_3D.FindBin(x_c, y_c, z_c) # bin from origianl TH3

      # should agree
      if binFromCoord!=ibin:
        print("@@@@ Mismatch @@@@")
        print("binFromCoord",binFromCoord)
        print("ibin",ibin)
        print("x", ix, x_l, x_r, x_c)
        print("y", iy, y_l, y_r, y_c)
        print("z", iz, z_l, z_r, z_c)

      binWidth = (z_r-z_l)*(y_r-y_l)*(x_r-x_l)

      #print("Filling ibin=%d"%(ibin))
      h_out.SetBinContent(ibin+1, h_3D.GetBinContent(ix, iy, iz))
      h_out.SetBinError(ibin+1, h_3D.GetBinError(ix, iy, iz))
      #h_out.SetBinError(ibin+1, 0.)

      # increase
      ibin += 1

f_out.cd()
h_3D.Write()
h_out.Write()
h_corr.Write()
f_out.Close()
