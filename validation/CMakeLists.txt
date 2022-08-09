
foreach(EXP 
	ANL BNL FNAL GGM BEBC 
	T2K MiniBooNE SciBooNE K2K MINERvA
	ArgoNeuT MicroBooNE)
	install(PROGRAMS 
		nuis-valid-gen-${EXP}
		nuis-valid-comp-${EXP}
		nuis-valid-plot-${EXP}
		DESTINATION var/validation)
endforeach()
