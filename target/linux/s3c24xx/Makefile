#
# Copyright (C) 2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=arm
BOARD:=s3c24xx
BOARDNAME:=ARMv9 JZ2440
FEATURES:=jffs2 squashfs
CFLAGS:=-Os -pipe -march=armv4t -mtune=arm920t -funit-at-a-time 
MAINTAINER:=jyin <jyin0813@gmail.com>

KERNEL_PATCHVER:=3.14

DEVICE_TYPE:=developerboard

include $(INCLUDE_DIR)/target.mk

define Target/Description
	Build JZ2440 Developboard
endef

KERNELNAME:=zImage 

$(eval $(call BuildTarget))
