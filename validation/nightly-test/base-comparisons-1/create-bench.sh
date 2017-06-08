#!/bin/sh

source build-script.sh
source run-nuisance.sh
source clean-script.sh

ln -sf $PWD/ $PWD/../base-benchmark/ 