#!/usr/bin/env bash

PIT_BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
IDF_TOOLS_PATH="${PIT_BASEDIR}/esp/tools"

echo "export PIT_BASEDIR=${PIT_BASEDIR}" >> $HOME/.bashrc 
echo "export IDF_TOOLS_PATH=${IDF_TOOLS_PATH}" >> $HOME/.bashrc

sudo apt-get install git wget flex bison gperf cmake ninja-build ccache libffi-dev libssl-dev libncurses5-dev libncursesw5-dev
sudo apt-get install python3 python3-pip python3-setuptools
sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 10

mkdir ./esp
cd ./esp
git clone -b release/v4.0 --recursive https://github.com/espressif/esp-idf.git


cd ./esp-idf
chmod 775 ./install.sh
./install.sh

chmod 775 ./export.sh
. ./export.sh

cd $PIT_BASEDIR/PIT
mkdir -p components
cd components
git clone -b idf-release/v4.0 https://github.com/espressif/arduino-esp32.git arduino 
cd arduino 
git submodule update --init --recursive 
cd $PIT_BASEDIR/PIT
idf.py menuconfig

bash
