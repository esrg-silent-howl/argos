obj-m := gpio.o
gpio-objs:= gpiomodule.o gpio_util.o utils.o

KDIR := /home/josep/Desktop/buildroot-2020.11.1/output/build/linux-custom
ARCH ?= arm
CROSS_COMPILE ?= /home/josep/Desktop/buildroot-2020.11.1/output/host/bin/arm-buildroot-linux-uclibcgnueabihf-

export ARCH CROSS_COMPILE

KBUILD_TARGETS	:= clean help modules modules_install

.PHONY: all $(KBUILD_TARGETS)

all: modules
$(KBUILD_TARGETS):
	$(MAKE) -C $(KDIR) M=$(shell pwd) $@
