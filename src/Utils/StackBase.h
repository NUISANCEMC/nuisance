#ifndef STACK_BASE_H
#define STACK_BASE_H

#include "MeasurementVariableBox.h"

class StackBase {
public:
	StackBase() {};
	~StackBase() {};

	virtual void AddMode(std::string name, std::string title, 
						int linecolor=1, int linewidth=1, int fillstyle=1001) {

		// int ncur = fAllLabels.size();
		fAllLabels.push_back(name);
		fAllTitles.push_back(title);
		fAllStyles.push_back(std::vector<int>(1, linecolor));
	}


	virtual bool IncludeInStack(TH1* hist) {return true;};
	virtual bool IncludeInStack(int index) {return true;};

	virtual void SetupStack(TH1* hist) {
		fTemplate = (TH1*) hist->Clone(fName.c_str());
		fTemplate->Reset();

		// Determine template dim
		fNDim = fTemplate->GetDimension();

		for (size_t i = 0; i < fAllLabels.size(); i++) {
			fAllHists.push_back( (TH1*) fTemplate->Clone( fAllLabels[i].c_str() ) );
		}
	};

	virtual void Scale(double sf, std::string opt=""){
		for (size_t i = 0; i < fAllLabels.size(); i++) {
			fAllHists[i]->Scale(sf, opt.c_str());
		}
	};

	virtual void Reset(){
		for (size_t i = 0; i < fAllLabels.size(); i++) {
			fAllHists[i]->Reset();
		}
	};

	virtual void FillStack(int index, double x, double y = 1.0, double z = 1.0, double weight = 1.0) {
		if (fNDim == 1)      fAllHists[index]->Fill(x, y);
		else if (fNDim == 2) ((TH2*)fAllHists[index])->Fill(x, y, z);
		else if (fNDim == 3) ((TH3*)fAllHists[index])->Fill(x, y, z, weight);
	}

	// Should add GetStack Function
	// Should add Scale Function

	virtual void Write() {
		THStack* st = new THStack();

		// Loop and add all histograms
		for (size_t i = 0; i < fAllLabels.size(); i++) {

			if (!IncludeInStack(fAllHists[i])) continue;
			if (!IncludeInStack(i)) continue;

			fAllHists[i]->SetTitle( fAllTitles[i].c_str() );
			fAllHists[i]->GetXaxis()->SetTitle( fXTitle.c_str() );
			fAllHists[i]->GetYaxis()->SetTitle( fYTitle.c_str() );
			fAllHists[i]->GetZaxis()->SetTitle( fZTitle.c_str() );
			fAllHists[i]->SetLineColor( fAllStyles[i][0] );
			st->Add(fAllHists[i]);
		}
		st->SetTitle(fTitle.c_str());
		st->SetName(fName.c_str());
		st->Write();
		delete st;
	};

	std::string fName;
	std::string fTitle;
	std::string fXTitle;
	std::string fYTitle;
	std::string fZTitle;

	TH1* fTemplate;
	int fNDim;

	// Maps incase we want to be selective
	std::vector< std::vector<int> >  fAllStyles;
	std::vector<std::string> fAllTitles;
	std::vector<std::string> fAllLabels;
	std::vector<TH1*> fAllHists;
};



class TrueModeStack : public StackBase {
public:

	// Individual constructor.
	TrueModeStack(std::string name, std::string title, TH1* hist){
		fName = name;
		fTitle = title;

		AddMode("CCQE", "CCQE", kBlue, 2, 3004);
		AddMode("2p2h", "2p2h", kRed, 2, 3004 );
		AddMode("RES", "RES",   kGreen, 2, 3004 );

		StackBase::SetupStack(hist);
	};

	void Fill(int mode, double x, double y = 1.0, double z = 1.0, double weight = 1.0) {
		StackBase::FillStack(ConvertModeToIndex(mode), x, y, z, weight);
	};

	// Should be kept in sync with constructor.
	int ConvertModeToIndex(int mode) {
		switch (abs(mode)) {
		case 1:  return 0; // CCQE
		case 2:  return 1; // 2p2h
		case 16: return 2; // RES
		default: return -1;
		}
	};
};

/*
class NuSpeciesStack : public StackBase {
public:
	SetupStack(TH1* hist) {
		AddMode("numu", "numu", kBlue, 2, 3004);
		AddMode("numubar", "numubar", kRed, 2, 3004 );
		AddMode("nue", "nue",   kGreen, 2, 3004 );
		StackBase::SetupStack(hist);
	};

	void NuSpeciesStack::FillStack(int species, double x, double y = 1.0, double z = 1.0, double weight = 1.0) {
		Stackbase::FillStack(ConvertSpeciesToIndex(species), x, y, z, weight);
	}

	int ConvertSpeciesToIndex(int species) {
		switch (species) {
		case 14: return 0;
		case -14: return 1;
		case 11: return 2;
		default: return -1;
		}
	};
};

class TargetStack : public StackBase {
public:
	SetupStack(TH1* hist) {
		AddMode("C", "C", kBlue, 2, 3004);
		AddMode("H", "H", kRed, 2, 3004 );
		AddMode("O", "O",   kGreen, 2, 3004 );
		StackBase::SetupStack(hist);
	};

	void NuSpeciesStack::FillStack(int species, double x,
	                               double y = 1.0, double z = 1.0,
	                               double weight = 1.0) {
		Stackbase::FillStack(ConvertTargetToIndex(species), x, y, z, weight);
	}

	int ConvertTargetToIndex(int target) {
		switch (species) {
		case 1000010010: return 0;
		case 1000: return 1;
		case 1000: return 2;
		default: return -1;
		}
	};
}

*/

// Setup pointer to template, and have reset,write,etc functions act on that.
class FakeStack : public StackBase {
public:
	FakeStack(TH1D* hist) {
		fTemplate =  (TH1*)hist;
		fNDim = fTemplate->GetDimension();
	}

	FakeStack(TH2D* hist) {
		fTemplate = (TH1*)hist;
		fNDim = fTemplate->GetDimension();
	}

	~FakeStack() {
		fTemplate = NULL;
		fNDim = 0;
	}

	void Fill(double x, double y = 1.0, double z = 1.0, double weight = 1.0) {
		if (fNDim == 1)      fTemplate->Fill(x, y);
		else if (fNDim == 2) ((TH2*)fTemplate)->Fill(x, y, z);
		else if (fNDim == 3) ((TH3*)fTemplate)->Fill(x, y, z, weight);
	}
	
	void Scale(double norm, std::string opt){
		fTemplate->Scale(norm, opt.c_str());
	}
	
	void Reset(){
		fTemplate->Reset();
	}
	
	void Write(){
		fTemplate->Write();
	}
};


#endif





