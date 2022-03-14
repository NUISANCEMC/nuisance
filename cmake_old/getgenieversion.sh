#!/bin/bash
LIBNAME=$(ls ${1}/lib/libGAlgorithm-*.so)
BASENAME="${LIBNAME##*-}"
BASENAME="${BASENAME//.so/}"
BASENAME="${BASENAME/./}"
BASENAME="${BASENAME/.*/}"
echo $BASENAME
