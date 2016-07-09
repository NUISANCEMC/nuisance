# Author: Callum Wilkinson <callum.wilkinson \at sheffield.ac.uk>

SHELL    = /bin/sh
NAME     = all
MAKEFILE = Makefile

# Include machine specific flags and locations (inc. files & libs)
#
include $(EXT_FIT)/make/Make.include

PACKAGE           = externalDataFit_MiniBooNE
DICTIONARY        = $(EXT_FIT)/src/MiniBooNE/_ROOT_DICT_ExternalDataFit_MiniBooNE
LIBNAME           = libExternalDataFit_MiniBooNE
EXTRA_EXT_LIBS    = 


all     : rootcint lib 
install : install-inc install-lib

# Include standard package makefile targets
#
include $(EXT_FIT)/make/Make.std-package-targets

FORCE:

