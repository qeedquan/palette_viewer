#!/bin/sh

qmake -project
echo 'QT += widgets gui' >> palette_viewer.pro
echo 'CONFIG += qt thread debug' >> palette_viewer.pro
echo 'QMAKE_CXXFLAGS += -O2 -std=gnu++14 -ggdb -g3' >> palette_viewer.pro
qmake
make clean
make
