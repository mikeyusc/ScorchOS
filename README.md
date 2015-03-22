WELCOME TO SCORCHOS 0.1.5
==========================
###_Another Take on Kernel Development_

Installation Instructions
-------------------------
###Emulator Testing

####Windows
ScorchOS 0.1 has been tested on the following packages:
Oracle Virtualbox
kqemu

Just mount `disk_images\scorch.img` as a floppy on your virtual machine at boot time

####Linux
ScorchOS *0.1* has been tested on the following packages:

* Oracle Virtualbox
* qemu (also requires additional kvm-pxe in Ubuntu Lucid)

As above, just mount `disk_images/scorch.img` as a floppy on the virtual machine in Virtualbox
But in Linux, you can run 'lintest.sh' to test the ScorchOS in qemu. Just make sure it's installed!

You can also run ScorchOS 0.1 off a real floppy disc on a real PC if you wish to
(at your own risk!). Check the 'Building' section for directions as to how to do this.

###Building

####Windows
Requires: nasm and i586-elf GCC cross compiler.
Run makefile from prompt as `make all` take your kernel from `bin/kernel.x` and put on a disk with grub, edit your menu.lst and boot.
If you need a virtual floppy device to moung the .img to put your kernel on, try vfdwin (Google is your friend here)   

####Linux
Requires: gcc, binutils and nasm, [naked gcc cross compiler](http://wiki.osdev.org/GCC_Cross-Compiler),and sudo

Run `linbuild.sh` with standard user privalages to build the kernel and add to the Scorch floppy image.

###Untested Emulators
For budding testers, ScorchOS has not been tested on the following packages:

* Windows
  * Microsoft Virtual PC 2007
  * VMware Player
  * VMware Server

* Linux
  * VMware Player
  * VMware Server
  * Bochs

* Mac / BSD / UNIX
  * (All packages - also no build support for this platform)

We assume ScorchOS could work with those software packages/platforms, but they are not officially supported.

####History
> *0.1.5* is a code cleanup, nothing should change from 0.1 except the codebase is much cleaner to look at, less
> comment bloat too. Everything is concise now. This is a minor patch before a major release, Hope to see you
> Soon!

Primis (10/31/2012)

> IMPORTANT: Linux support for Bochs has been broken on some distros.
> We advice you use the bochs-sdl plug-in on 32-bit systems & have edited bochsrc.txt
> accordingly (it is not possible to get things working on 64-bit setups.
> If development resumes on ScorchOS I will look further into this problem).
> Also, 'qemu' is no longer provided as a bash command on many modern distributions (such as Ubuntu 12.04 and later).
> Test scripts now launch the kvm directly.
>
> This release only provides minor patches to ensure that this 2 year-old kernel can still run on modern emulators!

Bob Moss (30/10/2012)


> This is the third kernel produced by the ScorchOS developers. Though you might not find anything too ground-breaking 
> here the aim has been to produce a sound base from which to build into the future. Please report any issues you 
> encounter.

Bob (20/12/2010)

>IMPORTANT: Do not expect the disk image you download from the SVN repository to have an up-to-date version
>of ScorchOS on it. It is only there so you do not have to keep regenerating images and adding grub to it!

Primis (12/20/2010) <- that's a US date stamp FYI

>Makefile has been added, currently expects that you have a cross compiler in your path called i586-elf-gcc and 
>i586-elf-ld. If you have a differnet cross compiler name such as MinGW, open the makefile, and modify the following:
>
>* `C_COMPILER = (your cross compiler's gcc here)`
>
>* `LINKER = (your cross linker here)`
Unknown
