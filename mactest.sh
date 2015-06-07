# ScorchOS Testing Script (Linux) - Requires Qemu (and additional kvm-pxe in Ubuntu Lucid)
# Copyright (C) 2008-2011 ScorchOS Developers
# For Terms see license.txt
#VBoxManage startvm "Test OS"

sudo qemu-system-i386 -fda disk_images/scorch.img -m 256 -monitor stdio -netdev tap,id=mynet0,script=tap-up,downscript=tap-down -net nic,model=rtl8139,netdev=mynet0 -usb -device ahci,id=ahci0 -drive if=none,file=/Users/mike/ScorchOS/disk_images/hd.raw,format=raw,id=drive-sata0-0-0 -device ide-drive,bus=ahci0.0,drive=drive-sata0-0-0,id=sata0-0-0 -boot order=ac
