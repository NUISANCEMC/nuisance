int MyPalette[255];

// Make the TPalette
void SetCustomPalette() {
  // Uncomment these to use the blue->white->red palette (good for correlation matrices)
  TColor::InitializeColors();
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;

  Double_t stops[NRGBs] = { 0.00, 0.25, 0.50, 0.75, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.25, 1.00, 1.00, 0.50 };
  Double_t green[NRGBs] = { 0.00, 0.25, 1.00, 0.25, 0.00 };
  Double_t blue[NRGBs]  = { 0.50, 1.00, 1.00, 0.25, 0.00 };

  int start = TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont); 
  for (int i = 0; i < NCont; i++) {
    MyPalette[i] = start+i;
  }
}
void FlatAnalysis(std::string filename, std::string novaname) {

  gStyle->SetOptStat(0);
  SetCustomPalette();

  TFile *file = new TFile(filename.c_str(), "OPEN");
  TTree *tree = (TTree*)file->Get("FlatTree_VARS");

  TFile *filenova = new TFile(novaname.c_str(), "OPEN");
  TTree *treenova = (TTree*)filenova->Get("FlatTree_VARS");

  // Mode, cc, PDGnu, tgtA, PDGLep
  // ELep, CosLep, Q2, q0, q3, Enu_true, Enu_QE, Q2_QE, W_nuc_rest, W, x, y
  // nfsp, px[nfsp], py[nfsp], pz[nfsp], E[nfsp], pdg[nfsp]
  // Weight (useless), InputWeight (useless), RWWeight (useless)
  // fScaleFactor, CustomWeight, CustomWeightArray[6]
  //
  // flagCCINC, flagNCINC
  // flagCCQE, flagCC0pi, flagCCQELike, flagNCEL, flagNC0pi
  // flagCCcoh, flagNCcoh,
  // flagCC1pip, flagNC1pip
  // flagCC1pim, flagNC1pim
  // flagCC1pi0, flagNC1pi0
  //
  double scalefactor = tree->GetMinimum("fScaleFactor");
  double scalefactornova = treenova->GetMinimum("fScaleFactor");
  if (scalefactor < 1E-42) {
    scalefactor *= 1E42;
    scalefactornova *= 1E42;
  }
  std::cout << scalefactor << std::endl;
  std::cout << scalefactornova << std::endl;
  
  //const int WeightNumbers = 7;
  const int WeightNumbers = 1;
  std::string WeightType[WeightNumbers];
  // The total weights
  //WeightType[0] = "1";
  WeightType[0] = "Weight";
  //WeightType[1] = "CustomWeight";
  //WeightType[2] = "CustomWeightArray[0]";
  //WeightType[3] = "CustomWeightArray[1]";
  //WeightType[4] = "CustomWeightArray[2]";
  //WeightType[5] = "CustomWeightArray[3]";
  //WeightType[6] = "CustomWeightArray[4]";
  //WeightType[7] = "CustomWeightArray[5]";

  std::string Names[WeightNumbers];
  //Names[0] = "No Weight";
  Names[0] = "Total Weight";
  //Names[2] = "M_{A}^{QE} Weight";
  //Names[3] = "Non-res Weight";
  //Names[4] = "RPA QE Weight";
  //Names[5] = "RPA RES Weight";
  //Names[6] = "MEC Weight";
  //Names[7] = "Total Weight";

  // What we want to draw
  std::vector<std::string> Draws2D;
  //Draws2D.push_back("ELep:CosLep");
  Draws2D.push_back("Q2:W");
  Draws2D.push_back("q0:q3");
  Draws2D.push_back("Q2:Enu_true");
  Draws2D.push_back("Eav:Enu_true");

  const int nModes = 8;
  std::string Modes[nModes];
  Modes[0]="Mode<30";
  Modes[1]="Mode==1";
  Modes[2]="Mode==2";
  Modes[3]="Mode==11";
  Modes[4]="Mode==12";
  Modes[5]="Mode==13";
  Modes[6]="Mode==21";
  Modes[7]="Mode==26";

  std::string ModeNames[nModes];
  ModeNames[0]="CC-Inclusive";
  ModeNames[1]="CCQE";
  ModeNames[2]="2p2h";
  ModeNames[3]="CC1#pi^{+}1p";
  ModeNames[4]="CC1#pi^{0}";
  ModeNames[5]="CC1#pi^{+}1n";
  ModeNames[5]="CC Multi-#pi";
  ModeNames[6]="CC DIS";

  std::string LimX2D[Draws2D.size()];
  //LimX2D[0]="0.0, 1.0";
  LimX2D[0]="1.0, 2.0";
  LimX2D[1]="0.0, 1.5";
  LimX2D[2]="1.0, 3.0";
  LimX2D[3]="1.0, 3.0";

  std::string LimY2D[Draws2D.size()];
  //LimY2D[0]="0.0, 3.0";
  LimY2D[0]="0.0, 2.0";
  LimY2D[1]="0.0, 1.5";
  LimY2D[2]="0.0, 2.0";
  LimY2D[3]="0.0, 0.5";

  std::vector<std::string> Draws1D;
  //Draws1D.push_back("ELep");
  //Draws1D.push_back("CosLep");
  Draws1D.push_back("Q2");
  Draws1D.push_back("W");
  Draws1D.push_back("x");
  Draws1D.push_back("y");
  Draws1D.push_back("Eav");

  std::string LimX1D[Draws1D.size()];
  LimX1D[0] = "0.0, 2.0";
  LimX1D[1] = "1.0, 3.0";
  LimX1D[2] = "0.0, 1.0";
  LimX1D[3] = "0.0, 1.0";
  LimX1D[4] = "0.0, 0.5";

  TCanvas *canv = new TCanvas("canv", "canv", 1024, 1024);
  canv->SetLeftMargin(canv->GetLeftMargin()*1.1);
  canv->SetBottomMargin(canv->GetBottomMargin()*0.8);
  canv->SetRightMargin(canv->GetRightMargin()*1.5);
  canv->SetTopMargin(canv->GetTopMargin()*0.8);
  std::string outname = filename;
  outname = outname.substr(0, outname.find(".root"));
  outname += "_output";
  canv->Print((outname+".pdf[").c_str());

  // The number of bins
  int nBins = 70;

  TFile *output = new TFile((outname+".root").c_str(), "RECREATE");

  // Draw the 2D
  for (int  i = 0; i < Draws2D.size(); ++i) {

    // Split the 2D into two parts
    std::string ypart = Draws2D[i].substr(0, Draws2D[i].find(":"));
    std::string xpart = Draws2D[i].substr(Draws2D[i].find(":")+1, Draws2D[i].length());

    for (int k = 0; k < nModes; ++k) {
      std::string ModeString = Modes[k];
      while (ModeString.find("(") != std::string::npos) {
        ModeString.replace(ModeString.find("("), 1, "_");
      }
      while (ModeString.find(")") != std::string::npos) {
        ModeString.replace(ModeString.find(")"), 1, "_");
      }
      while (ModeString.find("=") != std::string::npos) {
        ModeString.replace(ModeString.find("="), 1, "_");
      }

      std::stringstream refname;
      refname << ypart << ":" << xpart << ">>" << ypart << "_" << xpart << "_" << "noscale" << "_" << ModeString << "(" << nBins << ", ";
      std::stringstream ss2;
      ss2 << ", " << nBins << ", ";
      std::string DrawCmd = refname.str() + LimX2D[i] + ss2.str() + LimY2D[i] + std::string(")");
      std::string WeightDraw = Modes[k];
      file->cd();
      //std::cout << DrawCmd << std::endl;
      //std::cout << WeightDraw << std::endl;
      tree->Draw(DrawCmd.c_str(), WeightDraw.c_str(), "colz");
      std::string refname_str = ypart+"_"+xpart+"_noscale_"+ModeString;

      TH2D *refplot = (TH2D*)gDirectory->Get(refname_str.c_str())->Clone();
      //std::cout << refname_str << std::endl;
      refplot->Scale(scalefactor, "width");
      refplot->GetXaxis()->SetTitle(xpart.c_str());
      refplot->GetYaxis()->SetTitle(ypart.c_str());

      gStyle->SetPalette(51);
      // Make the string to draw
      std::stringstream ssnovaref;
      ssnovaref << ypart << ":" << xpart << ">>" << ypart << "_" << xpart << "_noscale_" << "CustomWeight" << "_" << ModeString << "_nova(" << nBins << ", ";

      filenova->cd();
      std::string DrawCmdnova = ssnovaref.str() + LimX2D[i] + ss2.str() + LimY2D[i] + std::string(")");
      std::string WeightDrawnova = Modes[k];
      //std::cout << DrawCmdnova << std::endl;
      //std::cout << WeightDrawnova << std::endl;
      treenova->Draw(DrawCmdnova.c_str(), WeightDrawnova.c_str(), "colz");
      std::string refname_strnova = ypart+"_"+xpart+"_noscale_CustomWeight_"+ModeString+"_nova";
      //std::cout << refname_strnova << std::endl;
      TH2D *refplotnova = (TH2D*)gDirectory->Get(refname_strnova.c_str())->Clone();
      refplotnova->Scale(scalefactornova, "width");
      refplotnova->GetXaxis()->SetTitle(xpart.c_str());
      refplotnova->GetYaxis()->SetTitle(ypart.c_str());

      // Loop over the different weights
      for (int j = 0; j < WeightNumbers; ++j) {

        std::string WeightString = WeightType[j];
        while (WeightString.find("[")!=std::string::npos) {
          WeightString.replace(WeightString.find("["), 1, "_");
        }
        while (WeightString.find("]")!=std::string::npos) {
          WeightString.replace(WeightString.find("]"), 1, "_");
        }

        // Make the string to draw
        std::stringstream ss;
        ss << ypart << ":" << xpart << ">>" << ypart << "_" << xpart << "_" << WeightString << "_" << ModeString << "(" << nBins << ", ";
        gStyle->SetPalette(51);

        DrawCmd = ss.str() + LimX2D[i] + ss2.str() + LimY2D[i] + std::string(")");
        WeightDraw = WeightType[j] + "*(" + Modes[k] + ")";
        canv->Clear();
        file->cd();
        tree->Draw(DrawCmd.c_str(), WeightDraw.c_str(), "colz");

        std::string getname = ypart+"_"+xpart+"_"+WeightString+"_"+ModeString;
        TH2D *plot = (TH2D*)gDirectory->Get(getname.c_str())->Clone();
        plot->Scale(scalefactor, "width");

        // Make the string to draw
        std::stringstream ssnova;
        ssnova << ypart << ":" << xpart << ">>" << ypart << "_" << xpart << "_" << "CustomWeight" << "_" << ModeString << "_nova(" << nBins << ", ";

        DrawCmdnova = ssnova.str() + LimX2D[i] + ss2.str() + LimY2D[i] + std::string(")");
        WeightDrawnova = std::string("CustomWeight") + "*(" + Modes[k] + ")";
        treenova->Draw(DrawCmdnova.c_str(), WeightDrawnova.c_str(), "colz");
        std::string getnamenova = ypart+"_"+xpart+"_"+"CustomWeight"+"_"+ModeString+"_nova";
        TH2D *plotnova = (TH2D*)gDirectory->Get(getnamenova.c_str())->Clone();

        plotnova->Scale(scalefactornova, "width");

        if (plot->Integral() == refplot->Integral()) {
          delete plot;
          continue;
        }

        double maximum = plot->GetMaximum();
        if (maximum < refplot->GetMaximum()) maximum = refplot->GetMaximum();
        plot->GetZaxis()->SetRangeUser(-1*maximum/1000.0, maximum);
        refplot->GetZaxis()->SetRangeUser(-1*maximum/1000.0, maximum);

        plotnova->GetZaxis()->SetRangeUser(-1*maximum/1000.0, maximum);
        refplotnova->GetZaxis()->SetRangeUser(-1*maximum/1000.0, maximum);

        plot->GetXaxis()->SetTitle(xpart.c_str());
        plot->GetYaxis()->SetTitle(ypart.c_str());
        canv->Clear();
        plot->Draw("colz");
        canv->Print((outname+".pdf").c_str());

        canv->Clear();
        refplot->Draw("colz");
        canv->Print((outname+".pdf").c_str());

        plotnova->GetXaxis()->SetTitle(xpart.c_str());
        plotnova->GetYaxis()->SetTitle(ypart.c_str());
        canv->Clear();
        plotnova->Draw("colz");
        canv->Print((outname+".pdf").c_str());

        canv->Clear();
        refplotnova->Draw("colz");
        canv->Print((outname+".pdf").c_str());

        gStyle->SetPalette(255, MyPalette);
        canv->Update();
        // Make a difference plot
        TH2D *diffplot = (TH2D*)plot->Clone();
        plot->Divide(refplot);
        //if (j == 0) maximum = plot->GetMaximum();
        plot->GetZaxis()->SetRangeUser(0, 2);

        std::string bla = (Names[j]+std::string(" MINERvA ")+ModeNames[k]);
        plot->SetTitle(bla.c_str());
        canv->Clear();
        plot->Draw("colz");
        canv->Print((outname+".pdf").c_str());

        output->cd();
        plot->Write();

        TH2D *diffplotnova = (TH2D*)plotnova->Clone();
        plotnova->Divide(refplotnova);
        //if (j == 0) maximum = plot->GetMaximum();
        plotnova->GetZaxis()->SetRangeUser(0, 2);

        std::string bla = (Names[j]+std::string(" NOvA ")+ModeNames[k]);
        plotnova->SetTitle(bla.c_str());
        canv->Clear();
        plotnova->Draw("colz");
        canv->Print((outname+".pdf").c_str());

        output->cd();
        plotnova->Write();

        // Make the difference plot
        for (int a = 0; a < plot->GetXaxis()->GetNbins(); ++a) {
          for (int b = 0; b < plot->GetYaxis()->GetNbins(); ++b) {
            if (diffplot->GetBinContent(a+1,b+1) != 0) {
              diffplot->SetBinContent(a+1, b+1, (diffplot->GetBinContent(a+1,b+1)-diffplotnova->GetBinContent(a+1, b+1))/diffplot->GetBinContent(a+1,b+1));
            } else {
              diffplot->SetBinContent(a+1, b+1, 0);
            }
          }
        }
        diffplot->GetZaxis()->SetRangeUser(-1.0, 1.0);
        diffplot->SetTitle((std::string("(MINERvA tuned-NOvA tuned)/MINERvA tuned ")+ModeNames[k]).c_str());
        diffplot->SetName((std::string(diffplot->GetName())+"_diff").c_str());

        canv->Clear();
        diffplot->Draw("colz");
        canv->Print((outname+".pdf").c_str());

        delete plot;
        delete plotnova;
        delete diffplot;
        delete diffplotnova;
      }
      delete refplot;
      delete refplotnova;
    }
  }

  nBins = 40;
  /// /////////////
  // Now do the 1D
  for (int i = 0; i < Draws1D.size(); ++i) {

    for (int k = 0; k < nModes; ++k) {
      std::string ModeString = Modes[k];
      while (ModeString.find("(") != std::string::npos) {
        ModeString.replace(ModeString.find("("), 1, "_");
      }
      while (ModeString.find(")") != std::string::npos) {
        ModeString.replace(ModeString.find(")"), 1, "_");
      }
      while (ModeString.find("=") != std::string::npos) {
        ModeString.replace(ModeString.find("="), 1, "_");
      }

      // Make the string to draw
      std::stringstream refname;
      refname << Draws1D[i] << ">>" << Draws1D[i] << "_" << "noscale" << "_" << ModeString << "(" << nBins << ", ";
      std::string DrawCmd = refname.str() + LimX1D[i] + std::string(")");

      std::string WeightDraw = std::string("(") + Modes[k] + ")";
      file->cd();
      tree->Draw(DrawCmd.c_str(), WeightDraw.c_str(), "colz");
      std::string refname_str = Draws1D[i]+"_noscale_"+ModeString;
      TH1D *refplot = (TH1D*)gDirectory->Get(refname_str.c_str())->Clone();
      refplot->Scale(scalefactor, "width");
      refplot->GetXaxis()->SetTitle(Draws1D[i].c_str());

      // Make the string to draw
      std::stringstream ssnovaref;
      ssnovaref << Draws1D[i] << ">>" << Draws1D[i] << "_noscale_" << "CustomWeight" << "_" << ModeString << "_nova(" << nBins << ", ";

      filenova->cd();
      std::string DrawCmdnova = ssnovaref.str() + LimX1D[i] + std::string(")");
      std::string WeightDrawnova = std::string("(") + Modes[k] + ")";
      treenova->Draw(DrawCmdnova.c_str(), WeightDrawnova.c_str(), "colz");
      std::string refname_strnova = Draws1D[i]+"_noscale_CustomWeight_"+ModeString+"_nova";
      //std::cout << refname_strnova << std::endl;
      TH1D *refplotnova = (TH1D*)gDirectory->Get(refname_strnova.c_str())->Clone();
      refplotnova->Scale(scalefactornova, "width");
      refplotnova->GetXaxis()->SetTitle(Draws1D[i].c_str());

      // Loop over the different weights
      for (int j = 0; j < WeightNumbers; ++j) {

        std::string WeightString = WeightType[j];
        while (WeightString.find("[")!=std::string::npos) {
          WeightString.replace(WeightString.find("["), 1, "_");
        }
        while (WeightString.find("]")!=std::string::npos) {
          WeightString.replace(WeightString.find("]"), 1, "_");
        }

        // Make the string to draw
        std::stringstream ss;
        ss << Draws1D[i] << ">>" << Draws1D[i] << "_" << WeightString << "_" << ModeString << "(" << nBins << ", ";

        DrawCmd = ss.str() + LimX1D[i] + std::string(")");

        WeightDraw = WeightType[j] + "*(" + Modes[k] + ")";
        canv->Clear();
        file->cd();
        tree->Draw(DrawCmd.c_str(), WeightDraw.c_str(), "colz");
        std::string getname = Draws1D[i]+"_"+WeightString+"_"+ModeString;
        TH1D *plot = (TH1D*)gDirectory->Get(getname.c_str())->Clone();
        plot->Scale(scalefactor, "width");

        // Make the string to draw
        std::stringstream ssnova;
        ssnova << Draws1D[i] << ">>" << Draws1D[i] << "_" << "CustomWeight" << "_" << ModeString << "_nova(" << nBins << ", ";
        DrawCmdnova = ssnova.str() + LimX1D[i] + std::string(")");
        WeightDrawnova = std::string("CustomWeight") + "*(" + Modes[k] + ")";
        canv->Clear();
        treenova->Draw(DrawCmdnova.c_str(), WeightDrawnova.c_str(), "colz");
        std::string getnamenova = Draws1D[i]+"_CustomWeight"+"_"+ModeString+"_nova";
        TH2D *plotnova = (TH2D*)gDirectory->Get(getnamenova.c_str())->Clone();
        plotnova->Scale(scalefactornova, "width");

        if (plot->Integral() == refplot->Integral()) {
          delete plot;
          continue;
        }

        double maximum = plot->GetMaximum();
        if (maximum < refplot->GetMaximum()) maximum = refplot->GetMaximum();
        maximum *= 1.5;
        plot->GetYaxis()->SetRangeUser(maximum, maximum);
        refplot->GetYaxis()->SetRangeUser(0, maximum);

        plotnova->GetYaxis()->SetRangeUser(0, maximum);
        refplotnova->GetYaxis()->SetRangeUser(0, maximum);

        plot->GetXaxis()->SetTitle(Draws1D[i].c_str());
        canv->Clear();
        refplot->SetLineColor(kRed);
        refplot->SetLineWidth(2);
        refplot->SetLineStyle(kDashed);
        refplot->Draw();

        plot->SetLineColor(kRed);
        plot->SetLineWidth(2);
        plot->Draw("same");

        plotnova->GetXaxis()->SetTitle(Draws1D[i].c_str());
        refplotnova->SetLineColor(kBlue);
        refplotnova->SetLineStyle(kDashed);
        refplotnova->SetLineWidth(2);
        refplotnova->Draw("same");

        plotnova->SetLineColor(kBlue);
        plotnova->SetLineWidth(2);
        plotnova->Draw("same");

        TLegend *leg = new TLegend(0.5, 0.5, 0.95, 0.95);
        leg->AddEntry(plot, "MINERvA tuned", "l");
        leg->AddEntry(refplot, "MINERvA untuned", "l");
        leg->AddEntry(plotnova, "NOvA tuned","l");
        leg->AddEntry(refplotnova, "NOvA untuned", "l");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetFillColor(0);
        leg->Draw("same");

        canv->Print((outname+".pdf").c_str());
        delete leg;

        plot->Divide(refplot);
        plot->GetYaxis()->SetRangeUser(0, 2);
        TLine *line = new TLine(plot->GetXaxis()->GetBinLowEdge(1), 1, plot->GetXaxis()->GetBinLowEdge(plot->GetXaxis()->GetNbins()+1), 1);
        line->SetLineStyle(kDashed);
        line->SetLineWidth(2);
        line->SetLineColor(kBlack);

        std::string bla = (Names[j]+std::string(" Comp ")+ModeNames[k]);
        plot->SetTitle(bla.c_str());
        plot->SetLineColor(kRed);
        plot->SetLineWidth(2);
        canv->Clear();
        plot->Draw();

        plotnova->Divide(refplotnova);
        plotnova->GetYaxis()->SetRangeUser(0, 2);
        TLine *line = new TLine(plot->GetXaxis()->GetBinLowEdge(1), 1, plot->GetXaxis()->GetBinLowEdge(plot->GetXaxis()->GetNbins()+1), 1);
        line->SetLineStyle(kDashed);
        line->SetLineWidth(2);
        line->SetLineColor(kBlack);

        std::string bla = (Names[j]+std::string(" NOvA ")+ModeNames[k]);
        plotnova->SetTitle(bla.c_str());
        plotnova->SetLineColor(kBlue);
        plotnova->SetLineWidth(2);
        plotnova->Draw("same");
        line->Draw("same");

        TLegend *leg = new TLegend(0.5, 0.5, 0.95, 0.95);
        leg->AddEntry(plot, "MINERvA tuned/untuned", "l");
        leg->AddEntry(plotnova, "NOvA tuned/untuned","l");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetFillColor(0);
        leg->Draw("same");

        canv->Print((outname+".pdf").c_str());

        output->cd();
        plot->Write();
        plotnova->Write();

        delete plot;
        delete plotnova;
        delete line;
      }
      delete refplot;
      delete refplotnova;
    }
  }

  canv->Print((outname+".pdf]").c_str());
  output->Close();
}
