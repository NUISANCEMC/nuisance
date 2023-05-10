// Copyright 2016-2021 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

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
#include "ComparisonRoutines.h"
#include "GeneralUtils.h"
#include "NuisConfig.h"
#include "NuisKey.h"
#include "FitLogger.h"
#include "ParserUtils.h"
#include "MeasurementBase.h"
#include "SampleList.h"

// void LoadSamples(std::vector<nuiskey> samplekeys) {
//   NUIS_LOG(MIN, "Loading Samples : " << samplekeys.size());
//   for (size_t i = 0; i < samplekeys.size(); i++) {
//     nuiskey key = samplekeys[i];

//     // Get Sample Options
//     std::string samplename = key.GetS("name");
//     std::string samplefile = key.GetS("input");
//     std::string sampletype = key.GetS("type");
//     std::string fakeData = "";

//     NUIS_LOG(MIN, "Loading Sample : " << samplename);

//     fOutputDir->cd();
//     MeasurementBase *NewLoadedSample = SampleUtils::CreateSample(key);

//     if (!NewLoadedSample) {
//       NUIS_ERR(FTL, "Could not load sample provided: " << samplename);
//       NUIS_ERR(FTL, "Check spelling with that in src/FCN/SampleList.cxx");
//       throw;
//     } else {
//       fSamples.push_back(NewLoadedSample);
//     }
//   }
// }

struct FuckOffMatrix{
   std::vector<uint32_t> event_id;
   std::vector<uint32_t> sample_id;
   std::vector<std::string> column_headers;
   std::vector<std::vector<double>> column_values;
};

std::vector<double>& GetColumn(FuckOffMatrix& m, std::string column_name){
   for (int i = 0; i < m.column_headers.size(); i++){
      if (!m.column_headers[i].compare(column_name)) return m.column_values[i];
   }
}

int AddColumn(FuckOffMatrix&m, std::string column_name){
   int index = m.column_headers.size();
   m.column_headers.push_back(column_name);
   m.column_values.push_back( std::vector<double>(0) );
   return index;
}

void Resize(FuckOffMatrix&m, int n){
   m.event_id.resize(n);
   m.sample_id.resize(n);
   for (int i = 0; i < m.column_headers.size(); i++){
      m.column_values[i].resize(n);
   }
}

void Print(FuckOffMatrix&m){
   for (int i = 0; i < m.column_headers.size(); i++){
      std::cout << m.column_headers[i] << " = " << m.column_values[i].size() << std::endl;
   }
}

void PrintTotalMatrix(FuckOffMatrix&m){
   std::cout << "# SAM ";
   for (int j = 0; j < m.column_headers.size(); j++){
      std::cout << m.column_headers[j] << " ";
   }
   std::cout << std::endl;
   for (int i = 0; i < m.event_id.size(); i++){
      if (i > 10 and i < m.event_id.size()-12) {
         std::cout << "......." << std::endl;
         i = m.event_id.size() - 11;
      }
      std::cout << m.event_id[i] << " " << m.sample_id[i] << " ";
      for (int j = 0; j < m.column_headers.size(); j++){
         std::cout << m.column_values[j][i] << " ";
      }
      std::cout << std::endl;
   }
}
//*******************************
int main(int argc, char *argv[]) {
  //*******************************

  // Program status;
  int status = 0;

  // Read input arguments such as card file, parameter arguments, and fit
  // routines
   std::cout << "[ NUISANCE ]: Starting nuishepdata.exe" << std::endl;
   std::string cardfile = argv[1];
       nuisconfig configuration = Config::Get();

   configuration.LoadSettings(cardfile,"");
   configuration.FinaliseSettings("test");


  std::vector<nuiskey> samplekeys = Config::QueryKeys("sample");
  if (!samplekeys.empty()) {
    NUIS_LOG(FIT, "Number of samples : " << samplekeys.size());
  }

   std::vector<MeasurementBase*> measures;
   for (size_t i = 0; i < samplekeys.size(); i++) {
      nuiskey key = samplekeys[i];
      MeasurementBase *NewLoadedSample = SampleUtils::CreateSample(key);
      measures.push_back(NewLoadedSample);
   }


   std::vector<FuckOffMatrix> matrices;
   for (int i = 0; i < measures.size(); i++){

      matrices.push_back( FuckOffMatrix() );

      MeasurementBase* exp = measures[i];
      uint8_t kXINDEX = AddColumn(matrices[i], exp->fName + "_X");
      uint8_t kSINDEX = AddColumn(matrices[i], exp->fName + "_S");
      uint8_t kWINDEX = AddColumn(matrices[i], exp->fName + "_W");
      Resize(matrices[i], exp->fInput->GetNEvents());

      // m.Resize(1000);

      // Array format
      // Event ID
      // HEPDATA_KIN1
      // HEPDATA_KIN2
      // HEPDATA_KIN3
      // HEPDATA_SIGN
      // HEPDATA_WEIG

      Print(matrices[i]);

      // FitEvent* cust_event = exp->fInput->GetEventPointer();
      FitEvent *cust_event = exp->fInput->FirstNuisanceEvent();
      int count = 0;
      while (cust_event) {
         exp->FillEventVariables(cust_event);
         bool sig = exp->isSignal(cust_event);
         if (count % 1000 == 0) std::cout << "SIGNAL : " << count << " " << sig << " " << exp->fXVar << " " << exp->fYVar << " " << exp->Weight << std::endl;
         cust_event = exp->fInput->NextNuisanceEvent();

         matrices[i].event_id[count] = count;
         matrices[i].sample_id[count] = i;
         matrices[i].column_values[kXINDEX][count] = exp->fXVar;
         matrices[i].column_values[kWINDEX][count] = exp->Weight;
         matrices[i].column_values[kSINDEX][count] = sig;
         count++;
      }

      PrintTotalMatrix(matrices[i]);
   }

   // NUIS_LOG(REC, " Reconfiguring sample " << fName);

//   // Reset Histograms
//   ResetExtraHistograms();
//   AutoResetExtraTH1();
//   this->ResetAll();

//   // FitEvent* cust_event = fInput->GetEventPointer();
//   int fNEvents = fInput->GetNEvents();
//   int countwidth = (fNEvents / 5);

//   // MAIN EVENT LOOP
//   FitEvent *cust_event = fInput->FirstNuisanceEvent();
//   int i = 0;
//   int npassed = 0;
//   while (cust_event) {
//     cust_event->RWWeight = fRW->CalcWeight(cust_event);
//     cust_event->Weight = cust_event->RWWeight * cust_event->InputWeight;

//     Weight = cust_event->Weight;

//     // Initialize
//     fXVar = -999.9;
//     fYVar = -999.9;
//     fZVar = -999.9;
//     Signal = false;
//     Mode = cust_event->Mode;

//     // Extract Measurement Variables
//     this->FillEventVariables(cust_event);
//     Signal = this->isSignal(cust_event);
//     if (Signal)
//       npassed++;

//     GetBox()->SetX(fXVar);
//     GetBox()->SetY(fYVar);
//     GetBox()->SetZ(fZVar);
//     GetBox()->SetMode(Mode);
//     // GetBox()->fSignal = Signal;

//     // Fill Histogram Values
//     GetBox()->FillBoxFromEvent(cust_event);
//     // this->FillExtraHistograms(GetBox(), Weight);
//     this->FillHistogramsFromBox(GetBox(), Weight);

//     // Print Out
//     if (LOG_LEVEL(REC) && countwidth > 0 && !(i % countwidth)) {
//       std::stringstream ss("");
//       ss.unsetf(std::ios_base::fixed);
//       ss << std::setw(7) << std::right << i << "/" << fNEvents << " events ("
//          << std::setw(2) << int(double(i) / double(fNEvents) * 100.) + 1
//          << std::left << std::setw(5) << "%) "
//          << "[S,X,Y,Z,M,W] = [" << std::fixed << std::setprecision(2)
//          << std::right << Signal << ", " << std::setw(5) << fXVar << ", "
//          << std::setw(5) << fYVar << ", " << std::setw(5) << fYVar << ", "
//          << std::setw(3) << (int)Mode << ", " << std::setw(5) << Weight << "] "
//          << std::endl;
//       NUIS_LOG(SAM, ss.str());
//     }

//     // iterate
//     cust_event = fInput->NextNuisanceEvent();
//     i++;
//   }





   std::cout << "Loaded Measurements : " << measures.size() << std::endl;

   return 0;
}
