#!/usr/bin/env bash

PORT = $1
BAUD = $2

cd $PIT_BASEDIR
. ./esp/esp-idf/export.sh
cd ./PIT
idf.py -p $PORT -b $BAUD flash