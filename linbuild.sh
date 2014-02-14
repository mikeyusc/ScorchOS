#!/bin/sh

# ScorchOS Build Script (Linux)
# Copyright (C) 2008-2011 ScorchOS Developers
# For Terms see License.txt

echo '> Starting Build...'

# This makes sure the user is root (needed for the loop-back mount of the floppy
# image I created earlier with a grub bootloader
#if test "`whoami`" != "root" ; then
#	echo "You must be logged in as root to build (for loopback mounting)"
#	echo "Enter 'su' or 'sudo bash' to switch to root"
#	exit
#fi

# This build script requires the presence of a cross-compiler
echo '> Assembling and Compiling ScorchOS Kernel...'
# (Primis) I've added the makefile support here to consolidate the build scripts
make all

#echo '> Adding Kernel to Floppy Image...'
#Time for that loop-back mount!
sudo rm -rf tmp-loop

mkdir tmp-loop && 
sudo mount -o loop -t vfat disk_images/scorch.img tmp-loop || exit
# Remove previous kernel.x before adding the new one...
cd tmp-loop
sudo rm -rf kernel.x || exit
cd ..
sudo cp bin/kernel.x tmp-loop/kernel.x || exit

# Move included program binaries onto the floppy image (will be used with ScorchOS 0.2)
#echo '> Moving binaries in Apps Directory to Floppy Image...'
#cp app/*.app tmp-loop

sleep 0.2

echo '> Unmounting Loop-back Floppy...'
# Unmount the loop-back floppy
sudo umount tmp-loop || exit
rm -rf tmp-loop

#echo '> Quick Cleanup...'
# A quick cleanup (comment this out while debugging)
make clean

echo '> Build Complete!'
