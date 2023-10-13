#include "TFile.h"
#include "TH1.h"
#include "TKey.h"

#include <iostream>

int main(int argc, char const *argv[]) {
  TFile *fin = TFile::Open(argv[1]);
  if (!fin || !fin->IsOpen()) {
    std::cout << "Failed to open " << argv[1] << " for reading." << std::endl;
    return 1;
  }
  TFile *fout = TFile::Open(argv[2], "CREATE");
  if (!fout || !fout->IsOpen()) {
    std::cout << "Failed to create " << argv[2] << "." << std::endl;
    return 1;
  }

  TList *list = fin->GetListOfKeys();
  if (!list) {
    std::cout << "File " << argv[1] << " is empty" << std::endl;
    return 1;
  }

  TIter next(list);
  TKey *key;
  TObject *obj;

  while (key = (TKey *)next()) {
    obj = key->ReadObj();
    if (!obj->InheritsFrom("TH1")) {
      continue;
    }

    TH1 *h = static_cast<TH1 *>(obj);
    h->SetDirectory(fout);

    if (h->GetDimension() != 1) {
      continue;
    }

    std::cout << "Dewidthing " << h->GetName() << std::endl;

    for (int i = 0; i < h->GetXaxis()->GetNbins(); ++i) {
      double bw = h->GetXaxis()->GetBinWidth(i + 1);

      if (h->GetSumw2N()) {
        h->SetBinError(i + 1, h->GetBinError(i + 1) * bw);
      }
      h->SetBinContent(i + 1, h->GetBinContent(i + 1) * bw);
    }
  }

  fout->Write();
  fout->Close();
}