if(NOT DEFINED SET_UP_PYTHIA6)

  if(DEFINED NEED_PYTHIA6 AND NEED_PYTHIA6)
    if(NOT DEFINED ENV{PYTHIA6} AND NOT DEFINED ENV{PYTHIA6_LIB})

      cmessage(FATAL_ERROR "Environment variable PYTHIA6/PYTHIA6_LIB is not defined. "
        "This must be set to point to a prebuilt NuWro instance.")

    endif()

    if(DEFINED ENV{PYTHIA6})
      set(PYTHIA6 $ENV{PYTHIA6})
    elseif(DEFINED ENV{PYTHIA6_LIB})
      set(PYTHIA6 $ENV{PYTHIA6_LIB})
    endif()
    set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -L${PYTHIA6} -lPythia6 -lgfortran")

  else()
    set(NEED_PYTHIA6 0)
  endif()

endif()
set(SET_UP_PYTHIA6 TRUE)
