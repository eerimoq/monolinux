LIBS += ml
ML = monolinux-c-library-0.7.0

packages: $(PACKAGES)/$(ML)

$(PACKAGES)/$(ML): $(ML_SOURCES)/$(ML).tar.gz
	$(MAKE) $(ML)-all

$(ML)-all:
	@echo "Building $(ML)."
	$(MAKE) $(ML)-unpack
	$(MAKE) $(ML)-build

$(ML)-clean:
	cd $(PACKAGES)/$(ML)

$(ML_SOURCES)/$(ML).tar.gz:
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/monolinux-c-library/archive/0.7.0.tar.gz

$(ML)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(ML).tar.gz

$(ML)-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch $(ML) && \
	cd $(ML) && \
	$(MAKE) library CC=$(CROSS_COMPILE)gcc ML_ROOT=. && \
	$(MAKE) install ML_ROOT=. PREFIX=$(SYSROOT)
