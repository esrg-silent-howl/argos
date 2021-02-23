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
cp "~/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/4. Support/dev_files/important_files/system/interfaces" board/raspberrypi/interfaces
cp "~/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/4. Support/dev_files/important_files/system/wpa_supplicant.conf" board/raspberrypi/wpa_supplicant.conf
cp "~/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/4. Support/dev_files/important_files/system/libtensorflow-lite.a" board/raspberrypi/libtensorflow-lite.a
cp "~/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/4. Support/dev_files/important_files/system/S00gpio" board/raspberrypi/S00gpio
cp "~/Desktop/ARGOS/argos/4. Implementation/2. Software Implementation/1. Main/drivers/gpio/gpio.ko" board/raspberrypi/gpio.ko


# copy interfaces and wpa_supplicant files from the board folder to the target
cp board/raspberrypi/interfaces ${TARGET_DIR}/etc/network/interfaces
cp board/raspberrypi/wpa_supplicant.conf ${TARGET_DIR}/etc/wpa_supplicant.conf
cp board/raspberrypi/libtensorflow-lite.a ${TARGET_DIR}/usr/lib/libtensorflow-lite.a
cp board/raspberrypi/SOOgpio ${TARGET_DIR}/etc/init.d/SOOgpio

# make S00gpio executable
chmod +x ${TARGET_DIR}/etc/init.d/SOOgpio

# create directory for gpio driver
if [ ! -d ${TARGET_DIR}/lib/modules/gpio ] then
	mkdir ${TARGET_DIR}/lib/modules/gpio
fi

cp board/raspberrypi/gpio.ko ${TARGET_DIR}/lib/modules/gpio/gpio.ko

# create /home directory for applications
if [ ! -d ${TARGET_DIR}/home ] then
	mkdir ${TARGET_DIR}/home
fi

# create first-boot signal in target
touch ${TARGET_DIR}/home/.firstboot

# cp board/raspberrypi/sshd_config ${TARGET_DIR}/etc/ssh/sshd_config
