STAGE = $(shell readlink -f build)
TOP = $(STAGE)/teeny-linux
BZIMAGE = $(TOP)/obj/linux-x86-allnoconfig/arch/x86/boot/bzImage
INITRAMFS = $(TOP)/initramfs.igz
APP = $(STAGE)/app
KERNEL = $(STAGE)/linux-4.10.6

.PHONY: all download kernel initrd run clean

all:
	$(MAKE) download
	$(MAKE) initrd kernel
	$(MAKE) run

download: $(KERNEL)

$(KERNEL):
	scripts/download.sh

kernel: $(BZIMAGE)

$(BZIMAGE): $(KERNEL)
	+scripts/kernel.sh

initrd:
	$(MAKE) $(INITRAMFS)

$(APP): src/main.c
	mkdir -p $(STAGE)
	gcc -Wall -Werror $< -static -o $@

$(INITRAMFS): $(APP)
	fakeroot scripts/create_initramfs.sh

run:
	scripts/run.sh

clean:
	rm -rf $(TOP) $(APP)
