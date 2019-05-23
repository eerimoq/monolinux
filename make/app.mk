BUILD = $(shell readlink -f build)
BZIMAGE = $(BUILD)/linux/arch/x86/boot/bzImage
INITRAMFS = $(BUILD)/initramfs.cpio
APP = $(BUILD)/app
LINUX_SRC = $(BUILD)/linux-$(ML_LINUX_VERSION)
MUSL_SRC = $(BUILD)/musl-$(ML_MUSL_VERSION)
MUSL_GCC = $(BUILD)/musl/bin/musl-gcc
MUSL_INSTALL_DIR=$(BUILD)/musl
MUSL_ROOT=$(BUILD)/musl-$(ML_MUSL_VERSION)
SCRIPTS_DIR = $(ML_ROOT)/scripts

INC += $(ML_ROOT)/src
SRC ?= \
	main.c \
	$(ML_ROOT)/src/ml.c \
	$(ML_ROOT)/src/ml_bus.c \
	$(ML_ROOT)/src/ml_log_object.c \
	$(ML_ROOT)/src/ml_message.c \
	$(ML_ROOT)/src/ml_network.c \
	$(ML_ROOT)/src/ml_queue.c \
	$(ML_ROOT)/src/ml_shell.c

ifeq ($(ARCH), arm64)
    MUSL_ARCH = ARCH=aarch64
endif

CC = $(CROSS_COMPILE)gcc

.PHONY: all unpack linux initrd run build clean app

all: build

unpack: $(LINUX_SRC) $(MUSL_SRC)

build:
	$(MAKE) unpack
	$(MAKE) musl
	$(MAKE) initrd linux

size:
	ls -l $(BZIMAGE) $(INITRAMFS)

run: build
	qemu-system-x86_64 \
	    -kernel $(BZIMAGE) \
	    -initrd $(INITRAMFS) \
	    -nographic -append "console=ttyS0"

clean:
	rm -rf $(BUILD)

$(MUSL_SRC):
	@echo "Unpacking $(ML_SOURCES)/musl-$(ML_MUSL_VERSION).tar.gz."
	mkdir -p $(BUILD)
	cd $(BUILD) && \
	tar xzf $(ML_SOURCES)/musl-$(ML_MUSL_VERSION).tar.gz

$(LINUX_SRC):
	@echo "Unpacking $(ML_SOURCES)/linux-$(ML_LINUX_VERSION).tar.xz."
	mkdir -p $(BUILD)
	cd $(BUILD) && \
	tar xJf $(ML_SOURCES)/linux-$(ML_LINUX_VERSION).tar.xz

linux: $(BZIMAGE)

musl: $(MUSL_GCC)

$(MUSL_GCC): $(MUSL_SRC)
	@echo "Building MUSL."
	mkdir -p $(MUSL_INSTALL_DIR)
	cd $(MUSL_ROOT) && \
	$(MUSL_ARCH) ./configure --disable-shared --prefix=$(MUSL_INSTALL_DIR) && \
	$(MAKE) $(MUSL_ARCH) && \
	$(MAKE) $(MUSL_ARCH) install

$(BZIMAGE): $(LINUX_SRC)
	@echo "Building Linux."
	cd $(BUILD)/linux-$(ML_LINUX_VERSION) && \
	$(MAKE) O=$(BUILD)/linux allnoconfig && \
	cp $(ML_LINUX_CONFIG) $(BUILD)/linux/.config && \
	$(MAKE) O=$(BUILD)/linux

initrd:
	$(MAKE) $(INITRAMFS)

app: $(APP)

$(APP): $(SRC)
	@echo "Building the application."
	mkdir -p $(BUILD)
	$(MUSL_GCC) -Wall -Wextra -Werror -O2 $(INC:%=-I%) $^ -static -o $@

$(INITRAMFS): $(APP)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD)
