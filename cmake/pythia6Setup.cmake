if(NOT DEFINED SET_UP_PYTHIA6)

  if(DEFINED NEED_PYTHIA6 AND NEED_PYTHIA6)
    if(NOT DEFINED ENV{PYTHIA6})

      cmessage(FATAL_ERROR "Environment variable PYTHIA6 is not defined. "
        "This must be set to point to a prebuilt NuWro instance.")

    endif()

    set(PYTHIA6 $ENV{PYTHIA6})
    set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -L${PYTHIA6} -lPythia6 -lgfortran")

  else()
    set(NEED_PYTHIA6 0)
  endif()

endif()
set(SET_UP_PYTHIA6 TRUE)
