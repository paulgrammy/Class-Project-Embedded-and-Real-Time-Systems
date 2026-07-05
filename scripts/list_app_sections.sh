#!/bin/bash

# check there's exactly one argument
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <Vitis application name>"
    exit 1
fi

# set Xilinx paths for GCC tools
. ~/Xilinx/Vitis/2023.1/settings64.sh

OLD_PATH=$(pwd)

# first try local path
APP_ELF="./$1.elf"
if [ -f "$APP_ELF" ]; then
    HEADER_PATH="./$1.sections.txt"
else
    # try absolute in project / relative to script 
    echo "ELF file $APP_ELF not local - trying absolute project path"

    BASH_SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
    cd $BASH_SCRIPT_DIR
    # now to project root path
    cd ..

    APP_ELF="./vitis_project_ees/$1/Debug/$1.elf"
    # check if the elf file exists
    if [ ! -f "$APP_ELF" ]; then
        echo "Error: ELF file $APP_ELF does not exist in Vitis project path relative to script either."
        exit 1
    fi
    HEADER_PATH="./vitis_project_ees/$1/Debug/$1.sections.txt"
fi


mb-objdump -h $APP_ELF | grep -Ev '^ {3,}' > $HEADER_PATH
echo ""  >> $HEADER_PATH
echo ""  >> $HEADER_PATH
echo ""  >> $HEADER_PATH
echo "-- AGAIN WITH SECTION ATTRIBUTES: --"  >> $HEADER_PATH
mb-objdump -h $APP_ELF >> $HEADER_PATH
code $HEADER_PATH &


cd $OLD_PATH