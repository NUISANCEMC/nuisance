#!/bin/sh

for name in $@;
do
    echo "-------------------------------------"
    echo "T2KRW Dials"
    echo "-------------------------------------"
    grep $name $T2KREWEIGHT/src/T2KSyst.h | grep break

    echo ""
    echo "-------------------------------------"
    echo "NEUT Dials"
    echo "-------------------------------------"
    grep $name $NEUT_REWEIGHT/NSyst.h | grep break

    echo ""
    echo "-------------------------------------"
    echo "NIWG Dials"
    echo "-------------------------------------"
    grep $name $NIWGREWEIGHT/NIWGSyst.h | grep break

    echo ""
    echo "-------------------------------------"
    echo "GENIE Dials"
    echo "-------------------------------------"
    grep $name $GENIE/src/ReWeight/GSyst.h | grep break

    echo ""
    echo "-------------------------------------"
    echo "Nuwro Dials"
    echo "-------------------------------------"
    grep $name $NUWRO/src/reweight/NuwroSyst.cc | grep break

    
done