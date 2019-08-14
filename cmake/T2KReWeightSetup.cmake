if(T2KREWEIGHT STREQUAL "")
  cmessage(FATAL_ERROR "Requested T2KReWeight support, but T2KREWEIGHT variable is not defined, either pass at configure time, or configure with it declared as an environment variable (rm CMakeCache.txt if it is currently defined as an environment variable and you still see this message.)")
endif()

SET(T2K_INCLUDE_DIRS)
SET(T2K_LINK_DIRS)
SET(T2K_LIBS)

LIST(APPEND T2K_INCLUDE_DIRS ${T2KREWEIGHT}/src)
LIST(APPEND T2K_LINK_DIRS ${T2KREWEIGHT}/lib)
LIST(APPEND T2K_LIBS T2KReWeight)

if(NIWG STREQUAL "")
  SET(USE_NIWG FALSE CACHE INTERNAL "Whether we are using the T2K NIWGReWeight. <FALSE>" FORCE)
else()
  SET(USE_NIWG TRUE CACHE INTERNAL "Whether we are using the T2K NIWGReWeight. <FALSE>" FORCE)
  LIST(APPEND T2K_INCLUDE_DIRS ${NIWG})
  LIST(APPEND T2K_LINK_DIRS ${NIWG})
  LIST(APPEND T2K_LIBS NIWGReWeight)
endif()

PrefixList(T2K_LINK_DIRS "-L" ${T2K_LINK_DIRS})
