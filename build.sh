#!/bin/sh

qmake -project .
echo 'CONFIG += qt thread debug' >> palette_viewer.pro
echo 'QMAKE_CXXFLAGS += -O0 -std=gnu++11' >> palette_viewer.pro
qmake
make clean
make
