EXE = $(BUILD)/app
BZIMAGE ?= $(BUILD)/linux/arch/x86/boot/bzImage
INITRAMFS = $(BUILD)/initramfs.cpio
INITRAMFS_FILES ?=
SCRIPTS_DIR = $(ML_ROOT)/scripts
CC = $(CROSS_COMPILE)gcc
CFLAGS += -O2
LDFLAGS += -static
SYSROOT = $(BUILD)/root
QEMU_NETDEV ?=
QEMU_DISKS ?=
BUILD = $(shell readlink -f build)
PACKAGES_DIR = $(BUILD)/packages

.PHONY: all run build packages

all: build

packages:
	$(MAKE) packages-rsync
	$(MAKE) packages-build

packages-rsync:

packages-build:

packages-clean:
	rm -rf $(BUILD)/packages $(BUILD)/root

build:
	$(MAKE) linux-all
	$(MAKE) packages
	$(MAKE) $(INITRAMFS)

run: build
	qemu-system-x86_64 \
	    -kernel $(BZIMAGE) \
	    -initrd $(INITRAMFS) \
	    -nographic \
	    -append "console=ttyS0" \
	    -device e1000,netdev=net0 \
	    $(QEMU_NETDEV) \
	    $(QEMU_DISKS:%=-drive format=raw,file=%)

size:
	ls -lh $(BZIMAGE) $(INITRAMFS) $(EXE)

$(INITRAMFS): $(EXE) $(INITRAMFS_FILES)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD) "$(INITRAMFS_FILES)"

include $(ML_ROOT)/make/packages/packages.mk

define PACKAGE_IMPORT_template
include $(ML_ROOT)/make/packages/$1.mk
endef
$(foreach package,$(PACKAGES),$(eval $(call PACKAGE_IMPORT_template,$(package))))

include $(ML_ROOT)/make/linux.mk
include $(ML_ROOT)/make/build.mk
