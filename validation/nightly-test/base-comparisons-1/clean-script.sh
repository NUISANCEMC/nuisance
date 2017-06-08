#!/bin/sh

rm -rf nuisance/build/
export GZIP=-9
tar -czvf nuisance_code.tar.gz ./nuisance/
rm -rf nuisance/
