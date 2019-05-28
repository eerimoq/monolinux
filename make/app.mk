BUILD = $(shell readlink -f build)
PACKAGES = $(BUILD)/packages
EXE = $(BUILD)/app
LINUX_SRC = $(BUILD)/linux-$(ML_LINUX_VERSION)
BZIMAGE ?= $(LINUX_SRC)/arch/x86/boot/bzImage
INITRAMFS = $(BUILD)/initramfs.cpio
SCRIPTS_DIR = $(ML_ROOT)/scripts
CC = $(CROSS_COMPILE)gcc
CFLAGS += -O2
LDFLAGS += -static
SYSROOT = $(BUILD)/root

.PHONY: all unpack kernel initrd run build packages

all: build

packages:

build:
	$(MAKE) packages
	$(MAKE) $(INITRAMFS)
	$(MAKE) $(LINUX_SRC)
	$(MAKE) kernel

run: build
	qemu-system-x86_64 \
	    -kernel $(BZIMAGE) \
	    -initrd $(INITRAMFS) \
	    -nographic -append "console=ttyS0"

$(LINUX_SRC):
	@echo "Unpacking $(ML_SOURCES)/linux-$(ML_LINUX_VERSION).tar.xz."
	mkdir -p $(BUILD)
	cd $(BUILD) && \
	tar xf $(ML_SOURCES)/linux-$(ML_LINUX_VERSION).tar.xz

$(LINUX_SRC)/.config: $(ML_LINUX_CONFIG) $(LINUX_SRC)
	@echo "Copying Linux kernel config $^ to $@."
	cp $(ML_LINUX_CONFIG) $@

kernel: $(LINUX_SRC)/.config
	@echo "Building the Linux kernel."
	$(MAKE) -C $(LINUX_SRC)

$(INITRAMFS): $(EXE)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD)

include $(ML_ROOT)/make/common.mk
