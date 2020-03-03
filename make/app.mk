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
	mkdir -p $(BUILD)
	if [ -n "$$(rsync -ariOu $(ML_SOURCES)/linux $(BUILD))" ] ; then \
	    echo "Building the Linux kernel." ; \
	    cp $(ML_LINUX_CONFIG) $(LINUX_SRC)/.config && \
	    $(MAKE) -C $(LINUX_SRC) ; \
	fi

linux-clean:
	rm -rf $(LINUX_SRC)

linux-build:
	echo "Building the Linux kernel."
	mkdir -p $(BUILD)
	rsync -ariOu $(ML_SOURCES)/linux $(BUILD) > /dev/null
	cp $(ML_LINUX_CONFIG) $(LINUX_SRC)/.config
	$(MAKE) -C $(LINUX_SRC)

$(INITRAMFS): $(EXE)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD) "$(INITRAMFS_FILES)"

include $(ML_ROOT)/make/build.mk
