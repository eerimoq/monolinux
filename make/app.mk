EXE = $(BUILD)/app
LINUX_SRC = $(BUILD)/linux
BZIMAGE ?= $(LINUX_SRC)/arch/x86/boot/bzImage
INITRAMFS = $(BUILD)/initramfs.cpio
INITRAMFS_FILES ?=
SCRIPTS_DIR = $(ML_ROOT)/scripts
CC = $(CROSS_COMPILE)gcc
CFLAGS += -O2
LDFLAGS += -static
SYSROOT = $(BUILD)/root
QEMU_DISKS ?= # mldisk.img
LINUX_RSYNC = $(BUILD)/linux/monolinux.rsync
LINUX_BUILD = $(BUILD)/linux/monolinux.build

.PHONY: all unpack kernel initrd run build packages $(LINUX_SRC)

all: build

packages:
	$(MAKE) packages-rsync
	$(MAKE) packages-build

packages-rsync:

packages-build:

packages-clean:
	rm -rf $(BUILD)/packages $(BUILD)/root

build:
	$(MAKE) linux-all packages
	$(MAKE) $(INITRAMFS)

run: build
	qemu-system-x86_64 \
	    -kernel $(BZIMAGE) \
	    -initrd $(INITRAMFS) \
	    -nographic -append "console=ttyS0" \
	    $(QEMU_DISKS:%=-drive format=raw,file=%)

linux-all:
	$(MAKE) linux-rsync
	$(MAKE) linux-build

linux-build: $(LINUX_BUILD)

linux-rsync:
	mkdir -p $(BUILD)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/linux $(BUILD))" ] ; then \
	    echo "linux sources updated" && \
	    touch $(LINUX_RSYNC) ; \
	fi

$(LINUX_BUILD): $(ML_LINUX_CONFIG)
$(LINUX_BUILD): $(LINUX_RSYNC)
	echo "Building the Linux kernel."
	cp $(ML_LINUX_CONFIG) $(LINUX_SRC)/.config
	$(MAKE) -C $(LINUX_SRC)
	touch $@

linux-clean:
	rm -rf $(LINUX_SRC)

linux-menuconfig:
	$(MAKE) -C $(LINUX_SRC) menuconfig
	cp $(LINUX_SRC)/.config $(ML_LINUX_CONFIG)

linux-nconfig:
	$(MAKE) -C $(LINUX_SRC) nconfig
	cp $(LINUX_SRC)/.config $(ML_LINUX_CONFIG)

$(INITRAMFS): $(EXE)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD) "$(INITRAMFS_FILES)"

include $(ML_ROOT)/make/build.mk
