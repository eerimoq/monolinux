LIBS += ml
ML_TAR_GZ = $(ML_SOURCES)/monolinux-c-library-0.7.0.tar.gz

packages: $(PACKAGES)/ml

$(PACKAGES)/ml:
	$(MAKE) ml-all

ml-all:
	@echo "Building ml."
	$(MAKE) ml-fetch
	$(MAKE) ml-unpack
	$(MAKE) ml-build

ml-clean:
	cd $(PACKAGES)/ml

ml-fetch: $(ML_TAR_GZ)

$(ML_TAR_GZ):
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/monolinux-c-library/archive/0.7.0.tar.gz

ml-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_TAR_GZ) && \
	mv monolinux-c-library-0.7.0 ml

ml-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch ml && \
	cd ml && \
	$(MAKE) library CC=$(CROSS_COMPILE)gcc ML_ROOT=. && \
	$(MAKE) install ML_ROOT=. PREFIX=$(SYSROOT)
