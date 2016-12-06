#!/bin/sh

# Set locale variables
export LANG=C
export LC_ALL=C

# Set surfaceless EGL platform
export EGL_PLATFORM=surfaceless

export HOME=$( su -c 'echo $HOME' ${TAC_USER} )
