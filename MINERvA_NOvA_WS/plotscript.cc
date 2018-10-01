void plotscript(std::string file1, std::string file2, std::string file3, std::string file4) {
  // file1 untuned MINERvA
  // file2 untuned NOvA
  // file3 tuned MINERvA
  // file4 tuned NOvA
  TFile *f1 = new TFile(file1.c_str(), "OPEN");
  TFile *f2 = new TFile(file2.c_str(), "OPEN");
  TFile *f3 = new TFile(file3.c_str(), "OPEN");
  TFile *f4 = new TFile(file4.c_str(), "OPEN");

  // Loop through all
  TIter next(f1->GetListOfKeys());
  TKey *key;
  std::string name;

  TCanvas *canv = new TCanvas("canv", "canv", 1024, 1024);
  canv->SetRightMargin(0.05);
  canv->Print((file1+"_"+file2+"_comp.pdf[").c_str());

  // Loop through all entries
  while ((key = (TKey*)next())) {

    // Get name of object
    name = std::string(key->GetName());

    if (name.find("_data") != std::string::npos && name.find("SHAPE") == std::string::npos && name.find("RATIO") == std::string::npos && name.find("sample") == std::string::npos) {
      if (name == "MiniBooNE_CCQE_XSec_1DQ2_nu_data" || name == "MiniBooNE_CCQE_XSec_1DQ2_antinu_data" || name == "MiniBooNE_CCQELike_XSec_1DQ2_nu_CCQELIKE_BKG_data" || name == "MiniBooNE_CCQELike_XSec_1DQ2_antinu_data") continue;
      if (name.find("_1D") != std::string::npos && name.find("MINERvA_CCinc_XSec") != std::string::npos) continue;

      std::string bla = name.substr(0, name.find("_data"));
      std::string dataName = name;

      // Get class of object
      TClass *cl = gROOT->GetClass(key->GetClassName());

      // Check TH1D inheritance and that both files contain the name
      if (!f2->GetListOfKeys()->Contains(name.c_str())) continue;
      if (!f3->GetListOfKeys()->Contains(name.c_str())) continue;
      if (!f4->GetListOfKeys()->Contains(name.c_str())) continue;

      if (std::string((key->ReadObj())->ClassName()).compare("TH1D") == 0) {
        std::cout << name << std::endl;

        bool error = false;
        TH1D *data = (TH1D*)(f1->Get(name.c_str()));
        if (data == NULL) {
          std::cerr << "no data" << std::endl;
          error = true;
        }

        TH1D *MC = (TH1D*)(f1->Get((bla+"_MC").c_str()));
        if (MC == NULL) {
          std::cerr << "no MC" << std::endl;
          error = true;
        }
        TH1D *MC2 = (TH1D*)(f2->Get((bla+"_MC").c_str()));
        if (MC2 == NULL) {
          std::cerr << "no MC2" << std::endl;
          error = true;
        }

        TH1D *MC3 = (TH1D*)(f3->Get((bla+"_MC").c_str()));
        if (MC3 == NULL) {
          std::cerr << "no MC3" << std::endl;
          error = true;
        }

        TH1D *MC4 = (TH1D*)(f4->Get((bla+"_MC").c_str()));
        if (MC4 == NULL) {
          std::cerr << "no MC4" << std::endl;
          error = true;
        }

        if (error) continue;

        data->SetMinimum(0);
        for (int i = 0; i < MC->GetNbinsX() + 1; i++) {
          MC->SetBinError(i+1, 0);
          MC2->SetBinError(i+1, 0);
        }

        canv->cd();
        data->Draw();
        MC->Draw("same,hist");
        MC2->Draw("same,hist");
        MC3->Draw("same,hist");
        MC4->Draw("same,hist");

        double maximum = data->GetMaximum();
        if (maximum < MC->GetMaximum()) maximum = MC->GetMaximum();
        data->GetYaxis()->SetRangeUser(0, maximum*1.7);

        data->SetLineWidth(2);
        MC->SetLineWidth(2);
        MC2->SetLineWidth(2);
        MC3->SetLineWidth(2);
        MC4->SetLineWidth(2);

        MC->SetLineColor(kRed-9);
        MC->SetMarkerStyle(0);
        MC->SetLineStyle(kDashed);

        MC2->SetLineColor(kRed+2);
        MC2->SetMarkerStyle(0);
        
        MC3->SetLineColor(kBlue-9);
        MC3->SetMarkerStyle(0);
        MC3->SetLineStyle(kDashed);

        MC4->SetLineColor(kBlue+2);
        MC4->SetMarkerStyle(0);

        TLegend *leg = new TLegend(0.6, 0.5, 0.95, 0.90);
        leg->AddEntry(data, "Data", "le");
        double chi2ndof = std::atof(MC->GetTitle())/MC->GetNbinsX();
        double chi2ndof2 = std::atof(MC2->GetTitle())/MC->GetNbinsX();
        double chi2ndof3 = std::atof(MC3->GetTitle())/MC->GetNbinsX();
        double chi2ndof4 = std::atof(MC4->GetTitle())/MC->GetNbinsX();
        leg->AddEntry(MC, Form("MINERvA untuned, #chi^{2}/ndof=%.2f", chi2ndof), "l");
        leg->AddEntry(MC2, Form("MINERvA tuned, #chi^{2}/ndof=%.2f", chi2ndof2), "l");
        leg->AddEntry(MC3, Form("NOvA untuned, #chi^{2}/ndof=%.2f", chi2ndof3), "l");
        leg->AddEntry(MC4, Form("NOvA tuned, #chi^{2}/ndof=%.2f", chi2ndof4), "l");
        leg->SetBorderSize(0);
        leg->SetFillStyle(0);
        leg->SetFillColor(0);
        leg->Draw("same");

        canv->Print((file1+"_"+file2+"_comp.pdf").c_str());
        delete leg;

        // Now draw the THStacks

      } else if (std::string((key->ReadObj())->ClassName()).compare("TH2D") == 0) {
        std::cout << name << std::endl;

        // Do projections of the TH2D
        bool error = false;
        TH2D *data2d = (TH2D*)(f1->Get(name.c_str()));
        if (data2d == NULL) {
          std::cerr << "no data" << std::endl;
          error = true;
        }

        TH2D *MC2d = (TH2D*)(f1->Get((bla+"_MC").c_str()));
        if (MC2d == NULL) {
          std::cerr << "no MC" << std::endl;
          error = true;
        }
        TH2D *MC22d = (TH2D*)(f2->Get((bla+"_MC").c_str()));
        if (MC22d == NULL) {
          std::cerr << "no MC2" << std::endl;
          error = true;
        }

        TH2D *MC32d = (TH2D*)(f3->Get((bla+"_MC").c_str()));
        if (MC32d == NULL) {
          std::cerr << "no MC3" << std::endl;
          error = true;
        }

        TH2D *MC42d = (TH2D*)(f4->Get((bla+"_MC").c_str()));
        if (MC42d == NULL) {
          std::cerr << "no MC4" << std::endl;
          error = true;
        }

        if (error) continue;

        /*
        data2d->SetMinimum(0);
        for (int i = 0; i < MC2d->GetXaxis()->GetNbins()+1; i++) {
          for (int j = 0; i < MC2d->GetYaxis()->GetNbins()+1; j++) {
            MC2d->SetBinError(i+1, j+1, 0);
            MC22d->SetBinError(i+1, j+1, 0);
            MC32d->SetBinError(i+1, j+1, 0);
            MC42d->SetBinError(i+1, j+1, 0);
          }
        }
        */
        // Do the projections
        int nbinsx = data2d->GetXaxis()->GetNbins();
        for (int i = 1; i < nbinsx+1; ++i) {
          TH1D *datax = data2d->ProjectionY(Form("%s_%i,%i", data2d->GetName(), 0, i), i, i);
          TH1D *mcx =   MC2d  ->ProjectionY(Form("%s_%i,%i", MC2d->GetName(),   1, i), i, i);
          TH1D *mc2x =  MC22d ->ProjectionY(Form("%s_%i,%i", MC22d->GetName(),  2, i), i, i);
          TH1D *mc3x =  MC32d ->ProjectionY(Form("%s_%i,%i", MC32d->GetName(),  3, i), i, i);
          TH1D *mc4x =  MC42d ->ProjectionY(Form("%s_%i,%i", MC42d->GetName(),  4, i), i, i);

          datax->Draw();
          mcx->Draw("same,hist");
          mc2x->Draw("same,hist");
          mc3x->Draw("same,hist");
          mc4x->Draw("same,hist");

          double maximum = datax->GetMaximum();
          if (maximum < MC->GetMaximum()) maximum = mcx->GetMaximum();
          datax->GetYaxis()->SetRangeUser(0, maximum*1.7);
          if (name == "MINERvA_CCinc_XSec_2DEavq3_nu_data") {
            datax->GetXaxis()->SetRangeUser(0, 0.6);
            datax->GetYaxis()->SetRangeUser(0, 6E-42);
          }

          mcx->SetLineColor(kRed-9);
          mcx->SetMarkerStyle(0);
          mcx->SetLineStyle(kDashed);

          mc2x->SetLineColor(kRed+2);
          mc2x->SetMarkerStyle(0);

          mc3x->SetLineColor(kBlue-9);
          mc3x->SetMarkerStyle(0);
          mc3x->SetLineStyle(kDashed);

          mc4x->SetLineColor(kBlue+2);
          mc4x->SetMarkerStyle(0);

          TLegend *leg = new TLegend(0.6, 0.5, 0.95, 0.90);
          int nbins = MC->GetXaxis()->GetNbins()*MC->GetYaxis()->GetNbins();
          double chi2ndof = std::atof(MC->GetTitle())/nbins;
          double chi2ndof2 = std::atof(MC2->GetTitle())/nbins;
          double chi2ndof3 = std::atof(MC3->GetTitle())/nbins;
          double chi2ndof4 = std::atof(MC4->GetTitle())/nbins;
          leg->AddEntry(datax, Form("Data %.2f-%.2f", data2d->GetXaxis()->GetBinLowEdge(i), data2d->GetXaxis()->GetBinLowEdge(i+1)), "le");
          leg->AddEntry(MC, Form("MINERvA untuned, #chi^{2}/ndof=%.2f", chi2ndof), "l");
          leg->AddEntry(MC2, Form("MINERvA tuned, #chi^{2}/ndof=%.2f", chi2ndof2), "l");
          leg->AddEntry(MC3, Form("NOvA untuned, #chi^{2}/ndof=%.2f", chi2ndof3), "l");
          leg->AddEntry(MC4, Form("NOvA tuned, #chi^{2}/ndof=%.2f", chi2ndof4), "l");
          leg->SetBorderSize(0);
          leg->SetFillStyle(0);
          leg->SetFillColor(0);
          leg->Draw("same");

          canv->Print((file1+"_"+file2+"_comp.pdf").c_str());
          delete leg;
        }


      }


    }

  }
  canv->Print((file1+"_"+file2+"_comp.pdf]").c_str());
}
