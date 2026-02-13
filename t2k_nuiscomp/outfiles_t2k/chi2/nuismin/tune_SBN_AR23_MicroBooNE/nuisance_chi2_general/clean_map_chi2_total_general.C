#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TVectorD.h>
#include <TMatrixD.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TList.h>
#include <TMath.h>
#include <iostream>
#include <fstream>
#include <regex>
#include <map>
#include <vector>
#include <string>

struct FileSet { //containers for observables
  std::vector<std::string> nuismin_files;
  std::vector<std::string> cov_files;
  std::string obs_tag;
  std::string obs_name;
};

TH1D* getH1(TFile* f, const std::string& key) { // check if 1D hist is not present
  auto h = (TH1D*) f->Get(key.c_str());
  if (!h)
    std::cerr << "[Warning post!] Missing TH1D: " << key << " in " << f->GetName() << std::endl;
  return h;
}

TH2D* getH2(TFile* f, const std::string& key) { // same check but for 2D hist
  auto h = (TH2D*) f->Get(key.c_str());
  if (!h) {
    std::string alt = key;
    size_t pos = alt.find("INVCOV");
    if (pos != std::string::npos) alt.replace(pos, 6, "INCOV");
    h = (TH2D*) f->Get(alt.c_str());
    if (h)
      std::cerr << "[Note] Using fallback key: " << alt << std::endl; // auto: so check for each and every relatable key
  }
  if (!h)
    std::cerr << "[Error!] Missing TH2D: " << key << " in " << f->GetName() << std::endl;
  return h;
}

int compute_ndof(const TH1D* h, int np) {
  int nz = 0;  // count non zero bins nz-np for dof
  for (int i = 1; i <= h->GetNbinsX(); ++i)
    if (h->GetBinContent(i) != 0) ++nz;
  return nz - np;
}

// =========================================================================== change following strings according to your experiment. Here its for MicroBooNE. 

void clean_map_chi2_total_general(
    const std::string& Experiment = "MicroBooNE",
    const std::string& exp_tag = "uboone",
    const std::string& Measurement = "CC1Mu1p",
    const std::string& DataType = "XSec",
    const std::string& DimensionVar = "1D",  // 
    const std::string& Neutrino = "nu",
    const char* dirpath = "/exp/dune/data/users/stiwari/detector_outfiles/MicroBooNE_outfiles/"
)
{
  const int n_params = 6;    // no of NUISANCE parameters; 6 in this case RPA_CCQE, b[4-1], NormCCMEC
  const double scale = 1e38;  // scaling factor; aftermath can be varified by checking order of magnitude of data, MC and cov matrix

  std::vector<std::pair<std::string, std::string>> obs_map = {
    {"pmu", "MuonMomentum"},
    {"thetamu", "MuonCosTheta"},
    {"ppro", "ProtonMomentum"},
    {"thetapro", "ProtonCosTheta"}
  };

  std::map<std::string, FileSet> all_sets; // IMP: this is keyed by above tags, don't mess up with this one
  // --- Regex for nuismin and covariance files (somehow it worked out; PS: If your code is working, don't touch it :)

  std::regex nuismin_allowed(
    "nuismin_6pLE_" + exp_tag + "_[a-z]+(_dcov_plus_pca1_offdiag|_dcov)?\\.root"
  );
  std::regex cov_full(
    "nuiscomp_(0pLE|6pLE)_" + exp_tag + "_[a-z]+(_dcov_plus_pca1_offdiag)?\\.root"
  );
  std::regex cov_diag(
    "nuiscomp_(0pLE|6pLE)_" + exp_tag + "_[a-z]+_dcov_manual\\.root"
  );

  // Scan directory
  TSystemDirectory dir("indir", dirpath); // Main root class, which helped the auto assignment...wraps input directory for listing
  TList* files = dir.GetListOfFiles(); // gives error if directory doesn't exist
  if (!files) { 
    std::cerr << "Directory not found or empty: " << dirpath << std::endl;
    return;
  }

  TIter next(files);
  TSystemFile* f;
  while ((f = (TSystemFile*)next())) {
    TString fname = f->GetName();
    if (!fname.EndsWith(".root")) continue;
    std::string name = fname.Data();

    for (auto& kv : obs_map) {  // for each map/tag, observable as name
      std::string tag = kv.first;
      std::string obsname = kv.second;
	// if name matches to nuismin then this
      if (std::regex_search(name, nuismin_allowed) && name.find(exp_tag + "_" + tag) != std::string::npos) {
        all_sets[tag].nuismin_files.push_back(std::string(dirpath) + "/" + name);
        all_sets[tag].obs_tag = tag;
        all_sets[tag].obs_name = obsname;
      }
	// if name matches to cov then this
      if ((std::regex_search(name, cov_full) || std::regex_search(name, cov_diag)) &&
          name.find(exp_tag + "_" + tag) != std::string::npos) {
        all_sets[tag].cov_files.push_back(std::string(dirpath) + "/" + name);
      }
    }
  }

 // Output csv file
  std::string outcsv = "chi2_map_" + Experiment + "_" + Measurement + "_" + DataType + "_" + DimensionVar + "_" + Neutrino + ".csv";
  std::ofstream csv(outcsv);
  csv << "observable,data_mc_file,cov_file,type,chi2,ndof,chi2_per_ndof,p_value\n";

  std::cout << "\n--- File Discovery Summary ---\n";
  for (auto& kv : all_sets) {
    auto& fs = kv.second;
    std::cout << "Observable: " << fs.obs_tag << std::endl;
    for (auto& n : fs.nuismin_files) std::cout << "  nuismin: " << n << std::endl;
    for (auto& c : fs.cov_files) std::cout << "  cov: " << c << std::endl;
  }
  std::cout << "-------------------------------\n";

  // Loop over all combinates to compute chi2 for each observable
  for (auto& kv : all_sets) { // iterate over each observable's fileset
    auto& fs = kv.second;
    if (fs.nuismin_files.empty() || fs.cov_files.empty()) continue;
    std::sort(fs.cov_files.begin(), fs.cov_files.end()); // what if list is empty? Skip it dude!

    // Opening data/MC hists
    for (auto& nuispath : fs.nuismin_files) {
      TFile* fData = TFile::Open(nuispath.c_str());

      // Correct histogram key structure
      std::string prefix = Experiment + "_" + Measurement + "_" + DataType + "_" + DimensionVar + fs.obs_name + "_";
      std::string data_key = prefix + Neutrino + "_data";
      std::string mc_key   = prefix + Neutrino + "_MC";
      std::string cov_key  = prefix + Neutrino + "_INVCOV";

       // extracting hists
      auto hData = getH1(fData, data_key); 
      auto hMC   = getH1(fData, mc_key);
      if (!hData || !hMC) { fData->Close(); continue; }

      int N = hData->GetNbinsX(); // n bins->length of vectors
      int ndof = compute_ndof(hData, n_params);

      for (auto& covpath : fs.cov_files) {
        bool is_diag_cov = (covpath.find("_dcov_manual") != std::string::npos); // identify and tag only diagonal ones
        TFile* fCov = TFile::Open(covpath.c_str()); // opening each cov file

        TH2D* hInvCov = getH2(fCov, cov_key);
        if (!hInvCov) { fCov->Close(); continue; }

	// for dcov
        double chi2 = 0.0;
        if (is_diag_cov) {
          for (int i = 1; i <= N; ++i) {
            double d = hData->GetBinContent(i) * scale;
            double m = hMC->GetBinContent(i) * scale;
            double diff = d - m;
            double invvar = hInvCov->GetBinContent(i, i); // 1/sig^2
            chi2 += diff * diff * invvar;
          }
        } else {
          TVectorD diff(N); // for fcm
          for (int i = 0; i < N; ++i) // data/MC is scale din each bin
            diff[i] = (hData->GetBinContent(i + 1) - hMC->GetBinContent(i + 1)) * scale;

          TMatrixD invCov(N, N); // fill INCOV from 2D hist
          for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
              invCov(i, j) = hInvCov->GetBinContent(i + 1, j + 1);

          TVectorD tmp = invCov * diff;
          chi2 = diff * tmp; // dot product
        }

        double chi2_ndf = (ndof > 0) ? chi2 / ndof : 0;
        double p = (ndof > 0) ? TMath::Prob(chi2, ndof) : 0;
        std::string type = is_diag_cov ? "diagonal" : "full";


	// decoration part
	std::cout << "\n=== Observable: " << fs.obs_name << " (" << fs.obs_tag << ") ===\n";
	std::cout << "Data/MC: " << nuispath << "\n";
	std::cout << "Cov file: " << covpath << " [" << type << "]\n";
	std::cout << "  chi2=" << chi2
          << " ndof=" << ndof
          << " chi2/ndof=" << chi2_ndf
          << " p=" << p << "\n";

        csv << fs.obs_name << ","
            << nuispath << ","
            << covpath << ","
            << type << ","
            << chi2 << ","
            << ndof << ","
            << chi2_ndf << ","
            << p << "\n";

        fCov->Close();
      }
      fData->Close();
    }
  }

  csv.close();
  std::cout << "\nExtended results saved to " << outcsv << "\n";
}

