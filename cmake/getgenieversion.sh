lib=$(ls $GENIE/lib/libGAlgorithm-*.so)
basename="${lib##*-}"
basename="${basename//.so/}"
basename="${basename/./}"
basename="${basename/.*/}"
echo $basename

