#!/bin/sh

qmake -project
echo 'QT += widgets gui' >> palette_viewer.pro
echo 'CONFIG += qt thread debug' >> palette_viewer.pro
echo 'QMAKE_CXXFLAGS += -std=gnu++14' >> palette_viewer.pro
qmake
make clean
make
