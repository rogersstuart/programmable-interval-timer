#!/usr/bin/env bash

. $PIT_BASEDIR/esp/esp-idf/export.sh
cd $PIT_BASEDIR/PIT
idf.py clean
idf.py build