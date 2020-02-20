SRC += lzma.c
INITRAMFS_FILES += build/linux-5.1.3/fs/ext4/ext4.ko
INITRAMFS_FILES += build/linux-5.1.3/fs/jbd2/jbd2.ko
INITRAMFS_FILES += build/linux-5.1.3/fs/mbcache.ko
QEMU_DISK = build/mldisk.img
QEMU_DISKS += $(QEMU_DISK)
QEMU_DISKS += fs.img
QEMU_DISKS += table.img
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
include $(ML_ROOT)/make/packages/detools-0.29.0.mk
include $(ML_ROOT)/make/packages/openssl-1.1.1b.mk
include $(ML_ROOT)/make/packages/zlib-1.2.11.mk
