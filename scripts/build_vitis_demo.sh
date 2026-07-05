#!/bin/bash


# localize
OLD_PATH=$(pwd)
BASH_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $BASH_SCRIPT_DIR

# Vitis 2023.1 environment for xsct
. ~/Xilinx/Vitis/2023.1/settings64.sh

mkdir -p ../vitis_project_demo

# run script
xsct build_vitis_demo.xsct.tcl

# create .metadata entry for "no new GUI question" if it doesn't exist
METAPATH="../vitis_project_demo/.metadata/.plugins/org.eclipse.core.runtime/.settings/"

if [ ! -d ${METAPATH} ]; then
    mkdir -p $METAPATH
fi

METAFILE="${METAPATH}/com.xilinx.sdx.core.prefs"
echo "TIP_OF_CHECK_RIGEL_DIALOG_OPEN=false" > ${METAFILE}
echo "eclipse.preferences.version=1" >> ${METAFILE}


METAFILE2="${METAPATH}/org.eclipse.ui.prefs"
echo "eclipse.preferences.version=1" > ${METAFILE2}
echo "showIntro=false" >> ${METAFILE2}


# run vitis
/home/esr/Xilinx/Vitis/2023.1/bin/vitis -workspace ../vitis_project_demo/ &> /dev/null & disown

cd $OLD_PATH


