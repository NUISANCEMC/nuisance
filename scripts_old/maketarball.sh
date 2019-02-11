#!/bin/sh
nuiscomp=$(which nuiscomp)
binfolder=$(dirname $nuiscomp)
echo $binfolder
allfiles=""
for file in $(find $binfolder/ -type f -perm /a+x -exec ldd {} \; | \grep so | sed -e '/^[^\t]/ d' | sed -e 's/\t//' | sed -e 's/.*=..//' | sed -e 's/ (0.*)//' | sort | uniq -c | sort -n);
do
    if [[ "$file" == "/usr/lib"* || "$file" == "/lib"* ]]
    then
	continue
    fi
    if [ -e $file ]
    then
	echo $file
	allfiles="$allfiles $file"
    fi
done
mkdir nuisance_reqs
mkdir ./nuisance_reqs/lib/
mkdir ./nuisance_reqs/bin/
cp -v $allfiles ./nuisance_reqs/lib/
cp -v $binfolder/../lib/ ./nuisance_reqs/lib/
cp -r $NUISANCE/data/ ./nuisance_reqs/data/
cp -r $NUISANCE/parameters/ ./nuisance_reqs/parameters/
cp -r $binfolder/* ./nuisance_reqs/bin/
echo "#!/bin/sh" > ./nuisance_reqs/setup.sh
echo 'export NUISANCE=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )' >> ./nuisance_reqs/setup.sh
echo 'export PATH=$NUISANCE/bin:$PATH' >> ./nuisance_reqs/setup.sh
echo 'export LD_LIBRARY_PATH=$NUISANCE/lib:$LD_LIBRARY_PATH' >> ./nuisance_reqs/setup.sh
#GZIP=-9
#tar -zcf ./nuisance_reqs.tar.gz ./nuisance_reqs/
#rm -rf ./nuisance_reqs