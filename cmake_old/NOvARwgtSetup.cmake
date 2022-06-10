if(NOVARWGT STREQUAL "")
  cmessage(FATAL_ERROR "Variable NOVARWGT is not defined. "
    "The location of a pre-built NOVARWGT install must be defined either as"
    " $ cmake -DNOVARWGT=/path/to/NOVARWGT or as an environment variable"
    " $ export NOVARWGT=/path/to/NOVARWGT")
endif()

LIST(APPEND EXTRA_CXX_FLAGS -D__NOVA_ENABLED__)

LIST(APPEND RWENGINE_INCLUDE_DIRECTORIES ${NOVARWGT}/inc)

LIST(APPEND EXTRA_LINK_DIRS ${NOVARWGT}/lib)

LIST(APPEND EXTRA_LIBS NOvARwgt)
