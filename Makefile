STAGE = $(shell readlink -f build)
TOP = $(STAGE)/teeny-linux
BZIMAGE = $(TOP)/obj/linux-x86-allnoconfig/arch/x86/boot/bzImage
INITRAMFS = $(TOP)/initramfs.igz
INITRAMFS_CPIO = $(TOP)/initramfs/initramfs.cpio
APP = $(STAGE)/app
KERNEL_SRC = $(STAGE)/linux-4.10.6
MUSL_SRC = $(STAGE)/musl-1.1.22
MUSL_GCC = $(STAGE)/musl/bin/musl-gcc

.PHONY: all download kernel initrd run build clean

all:
	$(MAKE) build
	$(MAKE) run

build:
	$(MAKE) download
	$(MAKE) musl
	$(MAKE) initrd kernel

download: $(KERNEL_SRC) $(MUSL_SRC)

$(MUSL_SRC) $(KERNEL_SRC):
	scripts/download.sh

kernel: $(BZIMAGE)

musl: $(MUSL_GCC)

$(MUSL_GCC): $(MUSL_SRC)
	+scripts/musl.sh

$(BZIMAGE): $(KERNEL_SRC)
	+scripts/kernel.sh

initrd:
	$(MAKE) $(INITRAMFS)

$(APP): src/main.c
	mkdir -p $(STAGE)
	$(MUSL_GCC) -Wall -Werror -O2 $< -static -o $@

$(INITRAMFS): $(APP)
	fakeroot scripts/create_initramfs.sh

size:
	ls -l $(BZIMAGE) $(INITRAMFS_CPIO)

run:
	scripts/run.sh

clean:
	rm -rf $(TOP) $(APP)
