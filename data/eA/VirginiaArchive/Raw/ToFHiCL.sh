#!/bin/bash

INPUT=${1}
OUTPUT=${2}

if [ ! -e ${INPUT} ]; then
  echo "Cannot find input file: \"${INPUT}\"."
  exit 1
fi

if [ -z ${OUTPUT} ]; then
  OUTPUT=/dev/stdout
fi


CURR_HASH=""

echo "datasets: [ " > ${OUTPUT}

while IFS= read -r LINE
do

  #Tidy line a bit
  LINE=$(echo ${LINE} | sed "s/^\s\+//g" | sed "s/\s\+/ /g")
  #Get CITE
  CITE=$(echo ${LINE} | cut -d " " -f 8)
  AUTHOR=$(echo ${CITE} | cut -d ":" -f 1)
  YEAR=$(echo ${CITE} | cut -d ":" -f 2 | cut -c1-4)
  TARGET_A=$(echo ${LINE} | cut -d " " -f 2)
  TARGET_Z=$(echo ${LINE} | cut -d " " -f 1)
  EIN=$(echo ${LINE} | cut -d " " -f 3)
  ANGOUT=$(echo ${LINE} | cut -d " " -f 4)

  ELOSS=$(echo ${LINE} | cut -d " " -f 5)
  XSEC=$(echo ${LINE} | cut -d " " -f 6)
  XSECErr=$(echo ${LINE} | cut -d " " -f 7)

  DATASETHASH=${CITE}_${TARGET_A}_${TARGET_Z}_${EIN}_${ANGOUT}

  ISNEW=0
  ISFIRST=0
  if [ -z ${CURR_HASH} ]; then
    CURR_HASH=${DATASETHASH}
    ISNEW=1
    ISFIRST=1
  elif [ ${DATASETHASH} != ${CURR_HASH} ]; then
    CURR_HASH=${DATASETHASH}
    ISNEW=1
    ISFIRST=0
  fi

  if [ ${ISNEW} == 1 ]; then
    echo "New data set = ${AUTHOR}, ${YEAR} on Z = ${TARGET_Z}, EIN = ${EIN}, ANGOUT = ${ANGOUT}."
    if [ ${ISFIRST} != 1 ]; then
      echo -e "\t\t]" >> ${OUTPUT}
      echo -e "\t}," >> ${OUTPUT}
    fi
    echo -e "\t{" >> ${OUTPUT}
    echo -e "\t\tAuthor: \"${AUTHOR}\"" >> ${OUTPUT}
    echo -e "\t\tYear: \"${YEAR}\"" >> ${OUTPUT}
    echo -e "\t\tCitation: \"${CITE}\"" >> ${OUTPUT}
    echo -e "\t\tTarget: [${TARGET_Z}, ${TARGET_A}]" >> ${OUTPUT}
    echo -e "\t\tEIn: ${EIN}" >> ${OUTPUT}
    echo -e "\t\tAngOut: ${ANGOUT}" >> ${OUTPUT}
    echo -e "\t\tData: [" >> ${OUTPUT}
  fi
  echo -e "\t\t\t[ ${ELOSS}, ${XSEC}, ${XSECErr} ]," >> ${OUTPUT}

done < ${INPUT}

echo -e "\t\t]" >> ${OUTPUT}
echo -e "\t}," >> ${OUTPUT}
echo -e "]" >> ${OUTPUT}
