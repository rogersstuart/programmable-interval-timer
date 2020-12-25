#!/usr/bin/env bash

sudo apt-get install git wget flex bison gperf cmake ninja-build ccache libffi-dev libssl-dev libncurses5-dev libncursesw5-dev
sudo apt-get install python3 python3-pip python3-setuptools
sudo update-alternatives --install /usr/bin/python python /usr/bin/python3 10

#create environment variables if needed

if [[ -z "${PIT_BASEDIR}" ]]; then
    PIT_BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
    echo "export PIT_BASEDIR=${PIT_BASEDIR}" >> $HOME/.bashrc 
fi

if [[ -z "${IDF_TOOLS_PATH}" ]]; then
    IDF_TOOLS_PATH="${PIT_BASEDIR}/esp/tools"
    echo "export IDF_TOOLS_PATH=${IDF_TOOLS_PATH}" >> $HOME/.bashrc
fi

#cleanup

if [[ -d "./esp" ]]; then
    rm -rf ./esp
fi

if [[ -d "./PIT/build" ]]; then
    rm -rf ./PIT/build
fi

if [[ -d "./PIT/components" ]]; then
    rm -rf ./PIT/components
fi

#set up IDF

mkdir ./esp
cd ./esp
git clone -b release/v4.0 --recursive https://github.com/espressif/esp-idf.git

cd ./esp-idf
chmod 775 ./install.sh
./install.sh

chmod 775 ./export.sh
. ./export.sh

#grab the Arduino component

cd $PIT_BASEDIR/PIT
mkdir -p components
cd components
git clone -b idf-release/v4.0 https://github.com/espressif/arduino-esp32.git arduino 
cd arduino 
git submodule update --init --recursive 

cd libraries

#download and configure libraries

git clone https://github.com/johnrickman/LiquidCrystal_I2C.git
git clone https://github.com/milesburton/Arduino-Temperature-Control-Library.git
git clone https://github.com/PaulStoffregen/OneWire.git
git clone https://github.com/igorantolic/ai-esp32-rotary-encoder.git
git clone https://github.com/ivanseidel/LinkedList.git

cd $PIT_BASEDIR

python library_prep.py

cd PIT

idf.py menuconfig

cd ..

bash
