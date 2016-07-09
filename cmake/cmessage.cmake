# Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

################################################################################
#    This file is part of NuFiX.
#
#    NuFiX is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    NuFiX is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
################################################################################
if(NOT WIN32)
  string(ASCII 27 Esc)
  set(CM_ColourReset "${Esc}[m")
  set(CM_ColourBold "${Esc}[1m")
  set(CM_Red "${Esc}[31m")
  set(CM_Green "${Esc}[32m")
  set(CM_Yellow "${Esc}[33m")
  set(CM_Blue "${Esc}[34m")
  set(CM_Magenta "${Esc}[35m")
  set(CM_Cyan "${Esc}[36m")
  set(CM_White "${Esc}[37m")
  set(CM_BoldRed "${Esc}[1;31m")
  set(CM_BoldGreen "${Esc}[1;32m")
  set(CM_BoldYellow "${Esc}[1;33m")
  set(CM_BoldBlue "${Esc}[1;34m")
  set(CM_BoldMagenta "${Esc}[1;35m")
  set(CM_BoldCyan "${Esc}[1;36m")
  set(CM_BoldWhite "${Esc}[1;37m")
endif()

message(STATUS "Setting up colored messages...")

function(cmessage)
  list(GET ARGV 0 MessageType)
  if(MessageType STREQUAL FATAL_ERROR OR MessageType STREQUAL SEND_ERROR)
    list(REMOVE_AT ARGV 0)
    message(${MessageType} "${CM_BoldRed}${ARGV}${CM_ColourReset}")
  elseif(MessageType STREQUAL WARNING)
    list(REMOVE_AT ARGV 0)
    message(${MessageType} "${CM_BoldYellow}${ARGV}${CM_ColourReset}")
  elseif(MessageType STREQUAL AUTHOR_WARNING)
    list(REMOVE_AT ARGV 0)
    message(${MessageType} "${CM_BoldCyan}${ARGV}${CM_ColourReset}")
  elseif(MessageType STREQUAL STATUS)
    list(REMOVE_AT ARGV 0)
    message(${MessageType} "${CM_Green}[INFO]:${CM_ColourReset} ${ARGV}")
  else()
    message(${MessageType} "${CM_Green}[INFO]:${CM_ColourReset} ${ARGV}")
  endif()
endfunction()


cmessage(STATUS "Done!")
