SRC += lzma.c
INITRAMFS_FILES += build/linux-5.1.3/fs/ext4/ext4.ko
INITRAMFS_FILES += build/linux-5.1.3/fs/jbd2/jbd2.ko
INITRAMFS_FILES += build/linux-5.1.3/fs/mbcache.ko
QEMU_DISK = build/mldisk.img
QEMU_DISKS += $(QEMU_DISK)
QEMU_DISKS += disk2/fs.img
QEMU_DISKS += disk2/table.img
CLEAN = $(QEMU_DISK)

default: $(QEMU_DISK)
	$(MAKE) all

$(QEMU_DISK):
	mkdir -p build
	create_file_storage.sh $@ detools

include $(ML_ROOT)/make/app.mk
include $(ML_ROOT)/make/packages/curl-7.65.0.mk
include $(ML_ROOT)/make/packages/heatshrink-0.4.1.mk
include $(ML_ROOT)/make/packages/xz-5.2.4.mk
include $(ML_ROOT)/make/packages/detools-0.45.0.mk
include $(ML_ROOT)/make/packages/openssl-1.1.1b.mk
include $(ML_ROOT)/make/packages/zlib-1.2.11.mk
include $(ML_ROOT)/make/packages/mbedtls-2.16.5.mk
include $(ML_ROOT)/make/packages/monolinux-c-library-0.7.0.mk
include $(ML_ROOT)/make/packages/bitstream-0.7.0.mk
include $(ML_ROOT)/make/packages/humanfriendly-0.3.0.mk
include $(ML_ROOT)/make/packages/async-0.6.0.mk
