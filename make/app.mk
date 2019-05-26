BUILD = $(shell readlink -f build)
PACKAGES = $(BUILD)/packages
EXE = $(BUILD)/app
BZIMAGE ?= $(BUILD)/linux/arch/x86/boot/bzImage
INITRAMFS = $(BUILD)/initramfs.cpio
LINUX_SRC = $(BUILD)/linux-$(ML_LINUX_VERSION)
SCRIPTS_DIR = $(ML_ROOT)/scripts
CC = $(CROSS_COMPILE)gcc
CFLAGS += -O2
LDFLAGS += -static

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
	tar xJf $(ML_SOURCES)/linux-$(ML_LINUX_VERSION).tar.xz

$(BUILD)/linux/.config: $(ML_LINUX_CONFIG)
	@echo "Copying Linux kernel config $^ to $@."
	mkdir -p $(BUILD)/linux
	cp $^ $@

kernel: $(BUILD)/linux/.config
	@echo "Building the Linux kernel."
	$(MAKE) -C $(LINUX_SRC) O=$(BUILD)/linux

$(INITRAMFS): $(EXE)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD)

include $(ML_ROOT)/make/common.mk
