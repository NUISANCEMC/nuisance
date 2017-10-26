// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include <iomanip>
#include <iostream>
#include <sstream>

#include "StdHepEvt.h"

// Include logging
#include "FitLogger.h"

StdHepReader::StdHepReader(){};

bool StdHepReader::SetBranchAddresses(TChain *chain) {
  bool ok = true;
  int SBAStatus = 0;
  SBAStatus = chain->SetBranchAddress("StdHepN", &StdHepN);
  ok = ok && (SBAStatus || SBAStatus == 5);
  if (!(!SBAStatus || SBAStatus == 5)) {
    ERR(WRN) << "Failed to set branch address for \"StdHepN\": " << SBAStatus
             << std::endl;
  }

  SBAStatus = chain->SetBranchAddress("StdHepPdg", StdHepPdg);
  ok = ok && (SBAStatus || SBAStatus == 5);
  if (!(!SBAStatus || SBAStatus == 5)) {
    ERR(WRN) << "Failed to set branch address for \"StdHepPdg\": " << SBAStatus
             << std::endl;
  }
  SBAStatus = chain->SetBranchAddress("StdHepStatus", StdHepStatus);
  ok = ok && (SBAStatus || SBAStatus == 5);
  if (!(!SBAStatus || SBAStatus == 5)) {
    ERR(WRN) << "Failed to set branch address for \"StdHepStatus\": "
             << SBAStatus << std::endl;
  }

  SBAStatus = chain->SetBranchAddress("StdHepP4", StdHepP4);
  ok = ok && (SBAStatus || SBAStatus == 5);
  if (!(!SBAStatus || SBAStatus == 5)) {
    ERR(WRN) << "Failed to set branch address for \"StdHepP4\": " << SBAStatus
             << std::endl;
  }
  return ok;
}

bool GiBUUStdHepReader::SetBranchAddresses(TChain *chain) {
  bool ok = true;
  int SBAStatus = 0;
  ok = ok && StdHepReader::SetBranchAddresses(chain);
  SBAStatus = chain->SetBranchAddress("GiBUU2NeutCode", &GiBUU2NeutCode);
  ok = ok && (SBAStatus || SBAStatus == 5);
  if (!(!SBAStatus || SBAStatus == 5)) {
    ERR(WRN) << "Failed to set branch address for \"GiBUU2NeutCode\": "
             << SBAStatus << std::endl;
  }
  SBAStatus = chain->SetBranchAddress("EvtWght", &EvtWght);
  ok = ok && (SBAStatus || SBAStatus == 5);
  if (!(!SBAStatus || SBAStatus == 5)) {
    ERR(WRN) << "Failed to set branch address for \"EvtWght\": " << SBAStatus
             << std::endl;
  }
  return ok;
}

std::string NegSpacer(double const &num) { return (num >= 0) ? " " : ""; }

std::ostream &operator<<(std::ostream &os, TLorentzVector const &tlv) {
  std::streamsize prec = os.precision();
  std::ios_base::fmtflags flags = os.flags();
  os.precision(2);
  os.flags(std::ios::scientific);
  os << "[" << NegSpacer(tlv[0]) << tlv[0] << "," << NegSpacer(tlv[1]) << tlv[1]
     << "," << NegSpacer(tlv[2]) << tlv[2] << "," << NegSpacer(tlv[3]) << tlv[3]
     << ":M(" << tlv.M() << ")]";
  os.precision(prec);
  os.flags(flags);
  return os;
}

std::string WriteGiBUUEvent(GiBUUStdHepReader const &gi) {
  std::stringstream ss("");

  ss << "[INFO]: contained " << gi.StdHepN
     << ", Event Weight: " << std::setprecision(3) << gi.EvtWght
     << ", NeutConventionReactionCode: " << gi.GiBUU2NeutCode
     << "\n\t[Lep In](" << std::setw(3)
       << gi.StdHepPdg[0] << ")  "
     << TLorentzVector(gi.StdHepP4[0][StdHepReader::kStdHepIdxPx],
                       gi.StdHepP4[0][StdHepReader::kStdHepIdxPy],
                       gi.StdHepP4[0][StdHepReader::kStdHepIdxPz],
                       gi.StdHepP4[0][StdHepReader::kStdHepIdxE])
     << std::endl;
  ss << "\t[Target] : " << gi.StdHepPdg[1] << std::endl;
  ss << "\t[Nuc In] :     "
     << TLorentzVector(gi.StdHepP4[3][StdHepReader::kStdHepIdxPx],
                       gi.StdHepP4[3][StdHepReader::kStdHepIdxPy],
                       gi.StdHepP4[3][StdHepReader::kStdHepIdxPz],
                       gi.StdHepP4[3][StdHepReader::kStdHepIdxE])
     << " (" << std::setw(4) << gi.StdHepPdg[3] << ")" << std::endl;

  for (Int_t stdHepInd = 4; stdHepInd < gi.StdHepN; ++stdHepInd) {
    ss << "\t[" << std::setw(2) << (stdHepInd - (4)) << "](" << std::setw(5)
       << gi.StdHepPdg[stdHepInd] << ")  "
       << TLorentzVector(gi.StdHepP4[stdHepInd][StdHepReader::kStdHepIdxPx],
                         gi.StdHepP4[stdHepInd][StdHepReader::kStdHepIdxPy],
                         gi.StdHepP4[stdHepInd][StdHepReader::kStdHepIdxPz],
                         gi.StdHepP4[stdHepInd][StdHepReader::kStdHepIdxE])
       << std::endl;
  }
  ss << "\t[Lep Out](" << std::setw(3)
       << gi.StdHepPdg[2] << ") "
     << TLorentzVector(gi.StdHepP4[2][StdHepReader::kStdHepIdxPx],
                       gi.StdHepP4[2][StdHepReader::kStdHepIdxPy],
                       gi.StdHepP4[2][StdHepReader::kStdHepIdxPz],
                       gi.StdHepP4[2][StdHepReader::kStdHepIdxE])
     << std::endl;
  return ss.str();
}
