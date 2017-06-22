#!/bin/bash
LIBNAME=$(ls ${GENIE}/lib/libGAlgorithm-*.so)
BASENAME="${LIBNAME##*-}"
BASENAME="${BASENAME//.so/}"
BASENAME="${BASENAME/./}"
BASENAME="${BASENAME/.*/}"
echo $BASENAME
