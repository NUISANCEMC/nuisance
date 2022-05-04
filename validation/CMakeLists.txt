
foreach(EXP T2K ANL)
	install(PROGRAMS 
		nuis-valid-gen-${EXP}
		nuis-valid-comp-${EXP}
		nuis-valid-plot-${EXP}
		DESTINATION var/validation)
endforeach()
