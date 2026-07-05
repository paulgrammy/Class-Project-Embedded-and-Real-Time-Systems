#!/bin/bash


# localize
OLD_PATH=$(pwd)
BASH_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $BASH_SCRIPT_DIR

cp ../project*/*.xsa ../hw/
cp ../project*/*.runs/impl_1/*wrapper.ltx ../hw/


cd $OLD_PATH


