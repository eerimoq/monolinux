BUILD = $(shell readlink -f build)
EXE = $(BUILD)/app
BZIMAGE ?= $(BUILD)/linux/arch/x86/boot/bzImage
INITRAMFS = $(BUILD)/initramfs.cpio
LINUX_SRC = $(BUILD)/linux-$(ML_LINUX_VERSION)
SCRIPTS_DIR = $(ML_ROOT)/scripts
CC = $(CROSS_COMPILE)gcc
LDFLAGS += -static

.PHONY: all unpack kernel initrd run build

all: build

build:
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
	tar xJf $(ML_SOURCES)/linux-$(ML_LINUX_VERSION).tar.xz

kernel:
	@echo "Building the Linux kernel."
	mkdir -p $(BUILD)/linux
	cp $(ML_LINUX_CONFIG) $(BUILD)/linux/.config
	$(MAKE) -C $(LINUX_SRC) O=$(BUILD)/linux

$(INITRAMFS): $(EXE)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD)

include $(ML_ROOT)/make/common.mk
