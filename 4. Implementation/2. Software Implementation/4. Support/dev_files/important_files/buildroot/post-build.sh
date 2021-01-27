#!/bin/sh
# [buildroot]/board/raspberrypi

set -u
set -e

# Add a console on tty1
if [ -e ${TARGET_DIR}/etc/inittab ]; then
    grep -qE '^tty1::' ${TARGET_DIR}/etc/inittab || \
	sed -i '/GENERIC_SERIAL/a\
tty1::respawn:/sbin/getty -L  tty1 0 vt100 # HDMI console' ${TARGET_DIR}/etc/inittab
fi

cp package/busybox/S10mdev ${TARGET_DIR}/etc/init.d/S10mdev
chmod 755 ${TARGET_DIR}/etc/init.d/S10mdev
cp package/busybox/mdev.conf ${TARGET_DIR}/etc/mdev.conf

# copy interfaces and wpa_supplicant files from desktop to the board folder
cp ~/Desktop/buildroot_files/interfaces board/raspberrypi/interfaces
cp ~/Desktop/buildroot_files/wpa_supplicant.conf board/raspberrypi/wpa_supplicant.conf

# copy interfaces and wpa_supplicant files from the board folder to the target
cp board/raspberrypi/interfaces ${TARGET_DIR}/etc/network/interfaces
cp board/raspberrypi/wpa_supplicant.conf ${TARGET_DIR}/etc/wpa_supplicant.conf

# create /home directory for applications
mkdir ${TARGET_DIR}/home

# create first-boot signal in target
touch ${TARGET_DIR}/home/.firstboot


# cp board/raspberrypi/sshd_config ${TARGET_DIR}/etc/ssh/sshd_config
