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

build:
	$(MAKE) $(LINUX_SRC) packages
	$(MAKE) $(INITRAMFS)

run: build
	qemu-system-x86_64 \
	    -kernel $(BZIMAGE) \
	    -initrd $(INITRAMFS) \
	    -nographic -append "console=ttyS0" \
	    $(QEMU_DISKS:%=-drive format=raw,file=%)

$(LINUX_SRC):
	$(MAKE) linux-all

linux-all:
	mkdir -p $(BUILD)
	if [ -n "$$(rsync -ariOu $(ML_ROOT)/3pp/linux $(BUILD))" ] ; then \
	    echo "Building the Linux kernel." ; \
	    cd $(BUILD) && \
	    cp $(ML_LINUX_CONFIG) $(LINUX_SRC)/.config && \
	    $(MAKE) -C $(LINUX_SRC) ; \
	fi

linux-clean:
	rm -rf $(LINUX_SRC)

linux-build:
	echo "Building the Linux kernel."
	mkdir -p $(BUILD)
	rsync -ariOu $(ML_ROOT)/3pp/linux $(BUILD) > /dev/null
	$(MAKE) -C $(LINUX_SRC)

$(INITRAMFS): $(EXE)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD) "$(INITRAMFS_FILES)"

include $(ML_ROOT)/make/build.mk
