#include "NUISANCEWeightCalcs.h"

GaussianModeCorr::GaussianModeCorr() {

	// Init
	fApply_CCQE = false;
	fGausVal_CCQE[kPosNorm] = 0.0;
	fGausVal_CCQE[kPosPq0]  = 1.0;
	fGausVal_CCQE[kPosWq0]  = 1.0;
	fGausVal_CCQE[kPosPq3]  = 1.0;
	fGausVal_CCQE[kPosWq3]  = 1.0;

	fApply_2p2h = false;
	fGausVal_2p2h[kPosNorm] = 0.0;
	fGausVal_2p2h[kPosPq0]  = 1.0;
	fGausVal_2p2h[kPosWq0]  = 1.0;
	fGausVal_2p2h[kPosPq3]  = 1.0;
	fGausVal_2p2h[kPosWq3]  = 1.0;

	fApply_2p2h_PPandNN = false;
	fGausVal_2p2h_PPandNN[kPosNorm] = 0.0;
	fGausVal_2p2h_PPandNN[kPosPq0]  = 1.0;
	fGausVal_2p2h_PPandNN[kPosWq0]  = 1.0;
	fGausVal_2p2h_PPandNN[kPosPq3]  = 1.0;
	fGausVal_2p2h_PPandNN[kPosWq3]  = 1.0;

	fApply_2p2h_NP = false;
	fGausVal_2p2h_NP[kPosNorm] = 0.0;
	fGausVal_2p2h_NP[kPosPq0]  = 1.0;
	fGausVal_2p2h_NP[kPosWq0]  = 1.0;
	fGausVal_2p2h_NP[kPosPq3]  = 1.0;
	fGausVal_2p2h_NP[kPosWq3]  = 1.0;

}

double GaussianModeCorr::CalcWeight(BaseFitEvt* evt) {

	FitEvent* fevt = static_cast<FitEvent*>(evt);
	double rw_weight = 1.0;

	// Get Neutrino
	FitParticle* pnu = fevt->PartInfo(0);
	int pdgnu = pnu->fPID;

	FitParticle* plep = fevt->GetHMFSParticle(abs(pdgnu) - 1);
	if (!plep) return 1.0;

	TLorentzVector q = pnu->fP - plep->fP;

	// Extra q0,q3
	double q0 = fabs(q.E())/1.E3;
	double q3 = fabs(q.Vect().Mag())/1.E3;

	// std::cout << "Got q0 q3 = " << q0 << " " << q3 << std::endl;

	// Apply weighting
	if (fApply_CCQE and abs(fevt->Mode) == 1){
		// std::cout << "Getting CCQE Weight" << std::endl;
		rw_weight *= GetGausWeight(q0,q3,fGausVal_CCQE);
	}

	if (fApply_2p2h and abs(fevt->Mode) == 2){
		// std::cout << "Getting 2p2h Weight" << std::endl;
		rw_weight *= GetGausWeight(q0,q3,fGausVal_2p2h);
	}

	if (fApply_2p2h_PPandNN and abs(fevt->Mode) == 2){
		// std::cout << "Getting 2p2h PPandNN Weight" << std::endl;
		rw_weight *= GetGausWeight(q0,q3,fGausVal_2p2h_PPandNN);
	}

	if (fApply_2p2h_NP and abs(fevt->Mode) == 2){
		// std::cout << "Getting 2p2h NP Weight" << std::endl;
		rw_weight *= GetGausWeight(q0,q3,fGausVal_2p2h_NP);
	}


	return rw_weight;
}

double GaussianModeCorr::GetGausWeight(double q0, double q3, double vals[]){


	double Norm = vals[kPosNorm];
	double Pq0  = vals[kPosPq0];
	double Wq0  = vals[kPosWq0];
	double Pq3  = vals[kPosPq3];
	double Wq3  = vals[kPosWq3];

	double w = Norm;
	w *= TMath::Gaus(q0, Pq0, Wq0);
	w *= TMath::Gaus(q3, Pq3, Wq3);

	// std::cout << "Returning " << Norm << " " << Pq0 << " " << Wq0 << " " << Pq3 << " " << Wq3 << " " << w << std::endl;

	return w;
}


void GaussianModeCorr::SetDialValue(std::string name, double val) {
	SetDialValue(Reweight::ConvDial(name, kCUSTOM), val);
}

void GaussianModeCorr::SetDialValue(int rwenum, double val) {

	int curenum = rwenum % 1000;

	// Check Handled
	if (!IsHandled(curenum)) return;


	// CCQE Setting
	for (int i = kGaussianCorr_CCQE_norm; i <= kGaussianCorr_CCQE_Wq3; i++) {
		if (i == curenum) {
			int index = i - kGaussianCorr_CCQE_norm;
			fGausVal_CCQE[index] = val;
			fApply_CCQE = true;
		}
	}

	// 2p2h Setting
	for (int i = kGaussianCorr_2p2h_norm; i <= kGaussianCorr_2p2h_Wq3; i++) {
		if (i == curenum) {
			int index = i - kGaussianCorr_2p2h_norm;
			fGausVal_2p2h[index] = val;
			fApply_2p2h = true;
		}
	}

	// 2p2h_PPandNN Setting
	for (int i = kGaussianCorr_2p2h_PPandNN_norm; i <= kGaussianCorr_2p2h_PPandNN_Wq3; i++) {
		if (i == curenum) {
			int index = i - kGaussianCorr_2p2h_PPandNN_norm;
			fGausVal_2p2h_PPandNN[index] = val;
			fApply_2p2h_PPandNN = true;
		}
	}

	// 2p2h_PN Setting
	for (int i = kGaussianCorr_2p2h_NP_norm; i <= kGaussianCorr_2p2h_NP_Wq3; i++) {
		if (i == curenum) {
			int index = i - kGaussianCorr_2p2h_NP_norm;
			fGausVal_2p2h_NP[index] = val;
			fApply_2p2h_NP = true;
		}
	}
}

bool GaussianModeCorr::IsHandled(int rwenum) {

	int curenum = rwenum % 1000;
	switch (curenum) {
	case kGaussianCorr_CCQE_norm:
	case kGaussianCorr_CCQE_Pq0:
	case kGaussianCorr_CCQE_Wq0:
	case kGaussianCorr_CCQE_Pq3:
	case kGaussianCorr_CCQE_Wq3:
	case kGaussianCorr_2p2h_norm:
	case kGaussianCorr_2p2h_Pq0:
	case kGaussianCorr_2p2h_Wq0:
	case kGaussianCorr_2p2h_Pq3:
	case kGaussianCorr_2p2h_Wq3:
	case kGaussianCorr_2p2h_PPandNN_norm:
	case kGaussianCorr_2p2h_PPandNN_Pq0:
	case kGaussianCorr_2p2h_PPandNN_Wq0:
	case kGaussianCorr_2p2h_PPandNN_Pq3:
	case kGaussianCorr_2p2h_PPandNN_Wq3:
	case kGaussianCorr_2p2h_NP_norm:
	case kGaussianCorr_2p2h_NP_Pq0:
	case kGaussianCorr_2p2h_NP_Wq0:
	case kGaussianCorr_2p2h_NP_Pq3:
	case kGaussianCorr_2p2h_NP_Wq3:
		return true;
	default:
		return false;
	}
}
