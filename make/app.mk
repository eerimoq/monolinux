BUILD = $(shell readlink -f build)
PACKAGES = $(BUILD)/packages
EXE = $(BUILD)/app
LINUX_SRC = $(BUILD)/linux-$(ML_LINUX_VERSION)
BZIMAGE ?= $(LINUX_SRC)/arch/x86/boot/bzImage
INITRAMFS = $(BUILD)/initramfs.cpio
INITRAMFS_FILES ?=
SCRIPTS_DIR = $(ML_ROOT)/scripts
CC = $(CROSS_COMPILE)gcc
CFLAGS += -O2
LDFLAGS += -static
SYSROOT = $(BUILD)/root
LINUX = linux-$(ML_LINUX_VERSION)
QEMU_DISKS ?= # mldisk.img

.PHONY: all unpack kernel initrd run build packages

all: build

packages:

build:
	$(MAKE) $(LINUX_SRC) packages
	$(MAKE) $(INITRAMFS)

run: build
	qemu-system-x86_64 \
	    -kernel $(BZIMAGE) \
	    -initrd $(INITRAMFS) \
	    -nographic -append "console=ttyS0" \
	    $(QEMU_DISKS:%=-drive format=raw,file=%)

$(LINUX_SRC): $(ML_SOURCES)/$(LINUX).tar.xz
	$(MAKE) $(LINUX)-all

$(LINUX)-all:
	@echo "Building the Linux kernel."
	$(MAKE) $(LINUX)-unpack
	$(MAKE) $(LINUX)-configure
	$(MAKE) $(LINUX)-build

$(LINUX)-clean:
	rm -rf $(LINUX_SRC)

$(ML_SOURCES)/$(LINUX).tar.xz:
	mkdir -p $(dir $@)
	wget -O $@ $(ML_LINUX_URL)

$(LINUX)-unpack:
	mkdir -p $(BUILD)
	cd $(BUILD) && \
	tar xf $(ML_SOURCES)/$(LINUX).tar.xz

$(LINUX)-configure:
	cp $(ML_LINUX_CONFIG) $(LINUX_SRC)/.config

$(LINUX)-build:
	$(MAKE) -C $(LINUX_SRC)

$(INITRAMFS): $(EXE)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD) "$(INITRAMFS_FILES)"

include $(ML_ROOT)/make/common.mk
