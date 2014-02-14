#!/bin/sh

# ScorchOS Manual Loop-back Unmount Script (Linux)
# Copyright (C) 2008-2011 ScorchOS Developers
# For Terms see license.txt

# Note that this umounts your previous loop-back mounts and saves the changes to your floppy image

echo '> Unmounting Loopback Floppy...'

if test "`whoami`" != "root" ; then
	echo "You must be logged in as root to unmount"
	echo "Enter 'su' or 'sudo bash' to switch to root"
	exit
fi

umount tmp-loop || exit
rm -rf tmp-loop
echo '> Done!'
