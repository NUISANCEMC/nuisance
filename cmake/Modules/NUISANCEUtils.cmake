if(NOT COMMAND EnsureVarOrEnvSet)
function(EnsureVarOrEnvSet VARNAME ENVNAME)
  set(options UNSET_IS_FATAL)
  cmake_parse_arguments(OPTS 
                      "${options}" 
                      "${oneValueArgs}"
                      "${multiValueArgs}" ${ARGN})

  if(NOT DEFINED ${VARNAME} OR "${${VARNAME}}x" STREQUAL "x")
    if(DEFINED ENV{${ENVNAME}} AND NOT "$ENV{${ENVNAME}}x" STREQUAL "x")
      set(${VARNAME} $ENV{${ENVNAME}} PARENT_SCOPE)
      return()
    endif()
  else()
    return()
  endif()

  if(OPTS_UNSET_IS_FATAL)
    cmessage(FATAL_ERROR "${VARNAME} undefined, either configure with -D${VARNAME}=<value> or set ${ENVNAME} into the environment")
  else()
    set(${VARNAME} ${VARNAME}-NOTFOUND PARENT_SCOPE)
  endif()

endfunction()
endif()


if(NOT COMMAND DefineEnabledRequiredSwitch)
  function(DefineEnabledRequiredSwitch VARNAME)
    if(NOT DEFINED ${VARNAME}_ENABLED OR "${${VARNAME}_ENABLED}x" STREQUAL "x")
      SET(${VARNAME}_ENABLED TRUE PARENT_SCOPE)
      SET(${VARNAME}_ENABLED TRUE)
    else()
      SET(${VARNAME}_REQUIRED ${${VARNAME}_ENABLED} PARENT_SCOPE)
      SET(${VARNAME}_REQUIRED ${${VARNAME}_ENABLED})
    endif()
  endfunction()
endif()