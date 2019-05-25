BUILD = $(shell readlink -f build)
BZIMAGE = $(BUILD)/linux/arch/x86/boot/bzImage
INITRAMFS = $(BUILD)/initramfs.cpio
APP = $(BUILD)/app
LINUX_SRC = $(BUILD)/linux-$(ML_LINUX_VERSION)
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

CC = $(CROSS_COMPILE)gcc

.PHONY: all unpack linux initrd run build clean app

all: build

unpack: $(LINUX_SRC)

build:
	$(MAKE) unpack
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

$(LINUX_SRC):
	@echo "Unpacking $(ML_SOURCES)/linux-$(ML_LINUX_VERSION).tar.xz."
	mkdir -p $(BUILD)
	cd $(BUILD) && \
	tar xJf $(ML_SOURCES)/linux-$(ML_LINUX_VERSION).tar.xz

linux: $(BZIMAGE)

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
	$(CC) -Wall -Wextra -Werror -O2 $(INC:%=-I%) $^ -static -lpthread -o $@

$(INITRAMFS): $(APP)
	@echo "Creating the initramfs."
	fakeroot $(ML_ROOT)/make/create_initramfs.sh $(BUILD)
