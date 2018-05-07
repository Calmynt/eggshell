#!/bin/sh
# This script is meant to switch makefiles.
# This is important for people who'd rather use GCC than Clang
# [Or are just intimidated by how to install Clang]

if [ ! -f Makefile-GCC ]; then
    mv Makefile Makefile-GCC
    mv Makefile-Clang Makefile
else
    mv Makefile Makefile-Clang
    mv Makefile-GCC Makefile
fi
