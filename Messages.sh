#! /usr/bin/env bash
$XGETTEXT `find . -name "*.cpp" -o -name "*.qml" | grep -v "/test/"` -o $podir/plasma_applet_milou_applet.pot
