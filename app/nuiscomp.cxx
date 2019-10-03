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
#include "ComparisonRoutines.h"

//*******************************
void printInputCommands() {
  //*******************************

  std::cout << "nuiscomp : NUISANCE Data Comparison App \n" << std::endl;

  std::cout
      << "# Running nuiscomp with a  card file #\n"
      << "######################################\n"
      << "nuiscomp   -c cardfile.xml [ -o outputfile.root ] [ -f routines ] [ "
         "-n maxevents ]    \n"
      << "         [ -i 'cardstructure' ] [ -d fakedata ] [ -q config=val ] [ "
         "+e/-e ] [ +v/-v ] \n"
      << "\n"
      << "# Running nuiscomp with structures at cmd line #\n"
      << "################################################\n"
      << "nuiscomp  -i 'cardstructure' -o outputfile.root [ -c cardfile.xml [ "
         "-f routines ] [ -n maxevents ]    \n"
      << "        [ -d fakedata ] [ -q config=val ] [ +e/-e ] [ +v/-v ] \n"
      << std::endl;

  sleep(4);
  std::cout
      << ""
      << "\n"
      << " \n"
      << "           -c cardfile.xml    : NUISANCE format card file defining "
         "comparisons.       \n"
      << " \n"
      << "           -o outputfile.root : Output file that histograms will be "
         "saved in. If a card file is \n"
      << "                                given but no output file this will "
         "default to cardfile.xml.root \n"
      << " \n"
      << "           -f routines        : Comma separated list of comparison "
         "routines to run in order.   \n"
      << "                                Allowed Routines :                   "
         "                          \n"
      << "                                Compare : Fixed comparison at "
         "nominal dial values.             \n"
      << " \n"
      << "           -n maxevents       : Set limit on the number of event "
         "entries to process. \n"
      << " \n"
      << "           -i \'cardstructure\' : Define card structure like those "
         "available in the standard NUISANCE \n"
      << "                                card format, but on the command line "
         "at runtime. MUST be enclosed   \n"
      << "                                in single quotation marks. See "
         "examples below for usage.            \n"
      << " \n"
      << "                                It is possible to entirely define "
         "the comparison using \' -i\' commands \n"
      << "                                without the need to write a card "
         "file explicitly. If you do this, \n"
      << "                                make sure to also use the \' -o\' "
         "flag to tell it where to go. \n"
      << " \n"
      << "           -d fakedata        : Define a fake data set to be used. "
         "All data in NUISANCE will be set \n"
      << "                                to the values defined in this fake "
         "data before comparisons are made. \n"
      << " \n"
      << "                                There are two possible methods. Fake "
         "data from MC or a previous file.\n"
      << "                                fakedata = \'MC\' : Sets the MC to "
         "the values defined by \'fake_parameters\' \n"
      << "                                                  shown in the "
         "examples below, and then sets the data  \n"
      << "                                                  to be equal to "
         "this MC prediction. \n"
      << "                                fakedata = \'file.root\' : Reads in "
         "the ROOT file at the specified path \n"
      << "                                                         assuming "
         "its a standard NUISANCE file. Takes \n"
      << "                                                         MC "
         "predictions in this file and uses them as \n"
      << "                                                         fake data. "
         "\n"
      << " \n"
      << "           -q config=val      : Overrides default configurations "
         "provided in the cardfile and in \n"
      << "                                '$NUISANCE/parameters/config.xml\'. "
         "Any config parameter can be set. \n"
      << "                                Examples : \n"
      << "                                \'-q VERBOSITY=4\' \n"
      << "                                \'-q EventManager=1\' \n"
      << "                                \'-q drawOpts=DATA/MC\' \n"
      << " \n"
      << "            +e/-e             : Increase/Decrease the default error "
         "verbosity by 1. \n"
      << " \n"
      << "            +v/-v             : Increase/Decrease the default "
         "logging verbosity by 1.\n"
      << " \n\n"
      << std::endl;

  sleep(4);
  std::cout
      << "# nuiscomp Running Examples #"
      << "############################# \n"
      << " \n"
      << " 1. Generate cardfile comparisons with increased verbosity and only "
         "50000 events \n\n"
      << "      nuiscomp -c cardfile.card -o mycomp.root -n 50000 +v +v \n"
      << " \n\n"
      << " 2. Generate a comparison to MiniBooNE data using simple structure, "
         "saving it to outfile.root \n\n"
      << "      nuiscomp -o outfile.root -i \'sample "
         "MiniBooNE_CCQE_XSec_1DQ2_nu NEUT:neutevents.root\' \n"
      << " \n\n"
      << " 3. Generate a comparison to MiniBooNE data using xml structure, "
         "reweight MaCCQE, and save the prediction to outfile.root \n\n"
      << "      nuiscomp -o outfile.root -i \'sample "
         "name=\"MiniBooNE_CCQE_XSec_1DQ2_nu\" "
         "input=\"NEUT:neutevents.root\"\'   \\ \n"
      << "                               -i \'sample "
         "name=\"MiniBooNE_CC1pip_XSec_1DQ2_nu\" "
         "input=\"NEUT:neutevents.root\"\' \\ \n"
      << "                               -i \'parameter name=\"MaCCQE\" "
         "nominal=\"1.0\" type=\"neut_parameter\"\' \n "
      << " \n\n"
      << " 4. Generate a comparison, using fake data from the MC predictions "
         "inside the fakedata.root \n\n"
      << "      nuiscomp -c cardfile.card -o myfakecomp.root -d fakedata.root "
         "\n"
      << " \n\n"
      << " 5. Generate a comparison using fake data defined on the command "
         "line use fake parameters \n\n"
      << "      nuiscomp -c cardfile.card -d MC -i \'fakeparameter "
         "name=\"MaCCQE\" nominal=\"1.0\"\' \n "
      << "                                      -i \'parameter name=\"MaCCQE\" "
         "nominal=\"1.0\" type=\"neut_parameter\"' "
      << " \n\n"
      << std::endl;

  sleep(4);
  std::cout << "# NUISANCE Card Format Structure Examples # \n"
            << "########################################### \n"
            << "\n"
            << "The NUISANCE card can be defined as a simple text file, or an "
               "xml file. \n"
            << "Examples for both with relevant structures are given below. \n"
            << std::endl;

  std::cout
      << "# XML Card File Example # \n"
      << "cardfile.xml: \n"
      << "<nuisance>"
      << "\n"
      << "<!-- CONFIG STRUCTURE --> \n"
      << "<!-- <config name=\'val\' /> --> \n"
      << "<config VERBOSITY=\'4\'/> \n"
      << "\n"
      << "<!-- Sample Structure --> \n"
      << "<!-- ID Corresponds to names given in src / FCN / SampleList.cxx --> "
         "\n"
      << "<!-- TYPE is the generator type (NEUT, NUWRO, GENIE, GIBUU). --> \n"
      << "<!-- FILE is the input generator events file. --> \n"
      << "<!-- TYPE is optional and used to define options for a class. e.g. "
         "FREE --> \n"
      << "<!-- NORM is optional and sets sample normalisations. --> \n"
      << "<!-- < sample name = \"ID\" input = \"TYPE : FILE\" type = \"TYPE\" "
         "norm = \"1.0\" /> --> \n"
      << "\n"
      << "<sample name=\"MiniBooNE_CCQE_XSec_1DQ2_nu\"   "
         "input=\"GENIE:genieevents.root\" /> \n"
      << "<sample name=\"MiniBooNE_CC1pip_XSec_1DQ2_nu\" "
         "input=\"GENIE:genieevents.root\" type=\"SHAPE\" /> \n"
      << "\n"
      << "\n"
      << "<!-- Parameter Structure --> \n"
      << "<!-- ID is the name of the dial in each generator RW engine --> \n"
      << "<!-- TYPE is the dial type (neut, newer, genie, niwg, t2k, custom, "
         "norm) --> \n"
      << "<!-- VAL is the nominal value in 1 - sigma variation for the "
         "comparison --> \n"
      << "<!-- <parameter name=\"ID\" type=\"TYPE_parameter\" nominal=\"VAL\" "
         "/> --> \n"
      << "\n"
      << "<parameter name=\"MaCCQE\" type=\"norm_parameter\" nominal=\"0.5\" "
         "/> \n"
      << "\n"
      << "<!-- Fake Parameter Structure --> \n"
      << "<!-- Sets values for fake data defined using the 'MC' flag. --> \n"
      << "<!-- ID is the dial name, it MUST be specified before hand using a "
         "normal parameter structure --> \n"
      << "<!-- VAL is the value to use for the fake data --> \n"
      << "<!-- <fakeparameter name=\"ID\" nominal=\"VAL\" /> --> \n"
      << "\n"
      << "<fakeparameter name=\"MaCCQE\" nominal=\"1.0\" /> \n"
      << "\n"
      << "</nuisance> \n\n"
      << std::endl;

  std::cout
      << "# Simple Card File Example # \n"
      << "cardfile.card: \n"
      << "\n"
      << "# CONFIG STRUCTURE  \n"
      << "# config name val \n"
      << "config VERBOSITY 4 \n"
      << "\n"
      << "# Sample Structure  \n"
      << "# ID Corresponds to names given in src/FCN/SampleList.cxx \n"
      << "# TYPE is the generator type (NEUT,NUWRO,GENIE,GIBUU). \n"
      << "# FILE is the input generator events file. \n"
      << "# TYPE is optional and used to define options for a class. e.g. FREE "
         " \n"
      << "# NORM is optional and sets sample normalisations. \n"
      << "# sample ID TYPE:FILE TYPE 1.0 \n"
      << "\n"
      << "sample MiniBooNE_CCQE_XSec_1DQ2_nu GENIE:genieevents.root \n"
      << "sample MiniBooNE_CC1pip_XSec_1DQ2_nu GENIE:genieevents.root SHAPE \n"
      << "\n"
      << "\n"
      << "# Parameter Structure \n"
      << "# ID is the name of the dial in each generator RW engine  \n"
      << "# TYPE is the dial type (neut,newer,genie,niwg,t2k,custom,norm)  \n"
      << "# VAL is the nominal value in 1-sigma variation for the comparison \n"
      << "# TYPE_parameter ID VAL  \n"
      << "\n"
      << "neut_parameter MaCCQE 0.5 \n"
      << "\n"
      << "# Fake Parameter Structure  \n"
      << "# Sets values for fake data defined using the ‘MC’ flag.  \n"
      << "# ID is the dial name, it MUST be specified before hand using a "
         "normal parameter structure  \n"
      << "# VAL is the value to use for the fake data  \n"
      << "# fake_parameter ID VAL \n"
      << "\n"
      << "fake_parameter MaCCQE 1.0 \n"
      << "\n"
      << std::endl;

  exit(-1);
};

//*******************************
int main(int argc, char *argv[]) {
  //*******************************

  // Program status;
  int status = 0;

  // If No Arguments print commands
  if (argc == 1)
    printInputCommands();

  for (int i = 1; i < argc; ++i) {
    // Cardfile
    if (!std::strcmp(argv[i], "-h"))
      printInputCommands();
    else
      break;
  }

  // Read input arguments such as card file, parameter arguments, and fit
  // routines
  NUIS_LOG(FIT, "Starting nuiscomp.exe");

  // Make minimizer class and run fit
  ComparisonRoutines *comp = new ComparisonRoutines(argc, argv);
  comp->Run();
  delete comp;

  // Show Final Status
  NUIS_LOG(FIT, "------------------------------------ -");
  NUIS_LOG(FIT, "Comparison Complete.");
  NUIS_LOG(FIT, "------------------------------------ -");

  return status;
}
