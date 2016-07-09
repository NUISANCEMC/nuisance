# Author: Callum Wilkinson <callum.wilkinson \at sheffield.ac.uk>

SHELL = /bin/sh
NAME = all
MAKEFILE = Makefile

# Include machine specific flags and locations (inc. files & libs)
#
include $(EXT_FIT)/make/Make.include

all: ExtFit_minimizer ExtFit_splines

# The following functions need retesting within the new framework
apps: ExtFit_workerNode ExtFit_functionPoint reweightArbitraryFlux genBinWeights genEventWeights ExtFit_throwNominal #scanChiSquare1D

ExtFit_splines: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_splines.cxx $(INCLUDES) 
	$(LD) $(LDFLAGS) ExtFit_splines.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_splines.exe

ExtFit_submitCard: FORCE
#	rm ${EXT_FIT}/bin/ExtFit_submitCard.sh
	cp ${EXT_FIT}/app/ExtFit_submitCard.sh ${EXT_FIT}/bin/ExtFit_submitCard.sh
	sed -i -e 's@EXT_FIT_DIR@${EXT_FIT}@g' ${EXT_FIT}/bin/ExtFit_submitCard.sh

ExtFit_submitJob: FORCE
#	rm ${EXT_FIT}/bin/ExtFit_submitJob1;2c.sh
	cp ${EXT_FIT}/app/ExtFit_submitJob.sh ${EXT_FIT}/bin/ExtFit_submitJob.sh
	sed -i -e 's@EXT_FIT_DIR@${EXT_FIT}@g' ${EXT_FIT}/bin/ExtFit_submitJob.sh

ExtFit_workerNode: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_workerNode.cxx $(INCLUDES) 
	$(LD) $(LDFLAGS) ExtFit_workerNode.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_workerNode.exe

ExtFit_functionPoint: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_functionPoint.cxx $(INCLUDES) 
	$(LD) $(LDFLAGS) ExtFit_functionPoint.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_functionPoint.exe

ExtFit_errorBand: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_errorBand.cxx $(INCLUDES) 
	$(LD) $(LDFLAGS) ExtFit_errorBand.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_errorBand.exe

ExtFit_scan: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_scan.cxx $(INCLUDES) 
	$(LD) $(LDFLAGS) ExtFit_scan.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_scan.exe

ExtFit_contours: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_contours.cxx $(INCLUDES) 
	$(LD) $(LDFLAGS) ExtFit_contours.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_contours.exe

ExtFit_throwNominal: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_throwNominal.cxx $(INCLUDES) 
	$(LD) $(LDFLAGS) ExtFit_throwNominal.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_throwNominal.exe


genEventWeights: FORCE
	$(CXX) $(CXXFLAGS) -c genEventWeights.cxx $(INCLUDES) 
	$(LD) $(LDFLAGS) genEventWeights.o $(LIBRARIES)  -o ${EXT_FIT}/bin/genEventWeights.exe

ExtFit_GLScontour: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_GLScontour.cxx $(INCLUDES)
	$(LD) $(LDFLAGS) ExtFit_GLScontour.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_GLScontour.exe

ExtFit_minimizer: FORCE
	$(CXX) $(CXXFLAGS) -c ExtFit_minimizer.cxx $(INCLUDES)
	$(LD) $(LDFLAGS) ExtFit_minimizer.o $(LIBRARIES)  -o ${EXT_FIT}/bin/ExtFit_minimizer.exe

minimizeCCQEWithReWeight: FORCE
	$(CXX) $(CXXFLAGS) -c minimizeCCQEWithReWeight.cxx $(INCLUDES)
	$(LD) $(LDFLAGS) minimizeCCQEWithReWeight.o $(LIBRARIES)  -o ${EXT_FIT}/bin/minimizeCCQEWithReWeight.exe

genBinWeights: FORCE
	$(CXX) $(CXXFLAGS) -c genBinWeights.cxx $(INCLUDES)
	$(LD) $(LDFLAGS) genBinWeights.o $(LIBRARIES)  -o ${EXT_FIT}/bin/genBinWeights.exe

reweightArbitraryFlux: FORCE
	$(CXX) $(CXXFLAGS) -c reweightArbitraryFlux.cxx $(INCLUDES)
	$(LD) $(LDFLAGS) reweightArbitraryFlux.o $(LIBRARIES)  -o ${EXT_FIT}/bin/reweightArbitraryFlux.exe

scanChiSquare1D: FORCE
	$(CXX) $(CXXFLAGS) -c scanChiSquare1D.cxx $(INCLUDES)
	$(LD) $(LDFLAGS) scanChiSquare1D.o $(LIBRARIES)  -o ${EXT_FIT}/bin/scanChiSquare1D.exe

purge: FORCE
purge: FORCE
	$(RM) *.o *~ core 

clean: FORCE
	rm ${EXT_FIT}/bin/ExtFit_submitJob.sh
	rm ${EXT_FIT}/bin/ExtFit_submitCard.sh
	$(RM) *.o *~ core
#	$(RM) *.root
	$(RM) *.exe
	$(RM) ${EXT_FIT}/bin/*.exe
FORCE:
