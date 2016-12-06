#!/bin/sh

# Set locale variables
export LANG=C
export LC_ALL=C

export HOME=$( su -c 'echo $HOME' ${TAC_USER} )
