SET(Experiments
	ANL
	ArgoNeuT
	BEBC
	BNL
	Electron
	GGM
	K2K
	MicroBooNE
	MINERvA
	MiniBooNE
	SciBooNE
	T2K)

foreach(EXP ${Experiments})
	if(NOT DEFINED ${EXP}_ENABLED)
		set(${EXP}_ENABLED TRUE)
	endif()

	if(${EXP}_ENABLED)
		add_subdirectory(${EXP})
	endif()
endforeach()

add_subdirectory(Config)
add_subdirectory(FCN)
add_subdirectory(FitBase)
add_subdirectory(InputHandler)
add_subdirectory(Logger)
add_subdirectory(MCStudies)
add_subdirectory(Reweight)
add_subdirectory(Routines)
add_subdirectory(Smearceptance)
add_subdirectory(Splines)
add_subdirectory(Statistical)
add_subdirectory(Utils)