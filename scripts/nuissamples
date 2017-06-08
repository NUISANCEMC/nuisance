#!/bin/sh

for line in $(grep compare $EXT_FIT/src/FCN/SampleList.cxx)
do
    if [[ $line != *"compare"* ]];
    then
	continue
    fi
    line=${line//\!name\.compare\(/}
    line=${line//\(/}
    line=${line//\)/}
    line=${line//\"/}

    if [[ $line != *"$1"* ]];
    then
	continue
    fi

    echo ${2}${line}${3}
done
