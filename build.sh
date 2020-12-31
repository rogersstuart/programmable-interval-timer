#!/usr/bin/env bash

clear

. $PIT_BASEDIR/esp/esp-idf/export.sh
cd $PIT_BASEDIR/PIT

if [[ $1 == "clean" ]]; then
	idf.py clean
fi

idf.py build
