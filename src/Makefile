# Author: Callum Wilkinson <callum.wilkinson \at sheffield.ac.uk>

SHELL    = /bin/sh
NAME     = all
MAKEFILE = Makefile

# Include machine specific flags and locations (inc. files & libs)
#
include $(EXT_FIT)/make/Make.include

PACKAGE           = externalDataFit
DICTIONARY        = $(EXT_FIT)/src/_ROOT_DICT_ExternalDataFit
LIBNAME           = libExternalDataFit
EXTRA_EXT_LIBS    = 


all     :  
	@echo "Building All of src subdirectories."
	@echo " - Building Base Classes and Utils..."
	cd Utils &&      make &&  cd ../
	cd FitBase &&    make &&  cd ../
	@echo " - Building Sample Folders..."
	cd ANL &&        make &&  cd ../
	cd BNL &&        make &&  cd ../
	cd FNAL &&       make &&  cd ../
	cd BEBC &&       make &&  cd ../
	cd GGM &&	 make &&  cd ../
	cd K2K &&        make &&  cd ../
	cd MINERvA &&    make &&  cd ../
	cd MiniBooNE &&  make &&  cd ../
	cd T2K &&        make &&  cd ../
	cd MCStudies &&  make &&  cd ../
	@echo " - Building FCN..."
	cd FCN &&        make &&  cd ../
	cd Minimizer &&  make &&  cd ../

clean: clean-files 

FCN:
	cd FCN &&  make &&  cd ../
	cd Minimizer &&  make &&  cd ../


clean-files: FORCE
	@echo "Cleaning All of src subdirectories."
	@echo " - Cleaning Base Classes and Utils..."
	cd Utils &&      make clean &&  cd ../
	cd FitBase &&    make clean &&  cd ../
	@echo " - Cleaning Sample Folders.."
	cd ANL &&        make clean &&  cd ../
	cd BNL &&        make clean &&  cd ../
	cd FNAL &&       make clean &&  cd ../
	cd BEBC &&       make clean &&  cd ../
	cd GGM &&	 make clean &&	cd ../
	cd K2K &&        make clean &&  cd ../
	cd MINERvA &&    make clean &&  cd ../
	cd MiniBooNE &&  make clean &&  cd ../
	cd T2K &&        make clean &&  cd ../
	cd MCStudies &&  make clean &&  cd ../
	@echo " - Cleaning FCN..."
	cd FCN &&        make clean &&  cd ../
	cd Minimizer &&  make clean &&  cd ../

# Include standard package makefile targets
#
include $(EXT_FIT)/make/Make.std-package-targets

FORCE:

