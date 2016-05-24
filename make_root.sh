#!bin/bash

dd if=/dev/zero of=root.bin bs=1M count=64
mkfs.ext2 -F root.bin

mkdir mnt
mount -o loop root.bin mnt

cd mnt
mkdir -p bin etc dev lib proc sbin sys tmp usr usr/bin usr/lib usr/sbin usr/include home home/root
cd ..
cp inittab mnt/etc/

cp busybox-1.24.1/busybox mnt/bin/
ln mnt/bin/busybox mnt/sbin/init
umount mnt
