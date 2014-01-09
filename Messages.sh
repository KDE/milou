#! /usr/bin/env bash
$XGETTEXT `find . -name "*.cpp *.qml"` -o $podir/milou.pot
rm -rf rc.cpp
