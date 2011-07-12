#!/bin/bash

DIR=$(dirname $0)
UIS=${DIR}/ui/
TARGET=${DIR}/src/ui_forms.py

> "${TARGET}"
for UI in ${UIS}*.ui;
do
	pyside-uic "${UI}" >> "${TARGET}"
done


