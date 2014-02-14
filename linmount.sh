#!/bin/sh

# ScorchOS Manual Loop-back Mount Script (Linux)
# Copyright (C) 2008-2011 ScorchOS Developers
# For Terms see license.txt

# Note that this loop-back mounts your floppy image to a folder named 'tmp-loop'
# For your changes to be saved, you then need to run the unmount script!

if test "`whoami`" != "root" ; then
	echo "You must be logged in as root to loopback mount"
	echo "Enter 'su' or 'sudo bash' to switch to root"
	exit
fi

echo '> Loop-Back Mounting Floppy Image...'
rm -rf tmp-loop

mkdir tmp-loop && mount -o loop -t vfat disk_images/scorch.img tmp-loop
echo '> Done!'
