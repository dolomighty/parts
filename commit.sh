#!/bin/sh
make clean
git reset
git add .
git status
git commit -m $1
