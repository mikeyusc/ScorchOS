# ScorchOS Testing Script (Linux) - Requires Qemu (and additional kvm-pxe in Ubuntu Lucid)
# Copyright (C) 2008-2011 ScorchOS Developers
# For Terms see license.txt

qemu-system-i386 -fda disk_images/scorch.img -m 256
