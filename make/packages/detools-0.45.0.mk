LIBS += detools
DETOOLS = detools-0.45.0

packages: $(PACKAGES)/$(DETOOLS)

$(PACKAGES)/$(DETOOLS): $(PACKAGES)/xz-5.2.4
$(PACKAGES)/$(DETOOLS): $(PACKAGES)/heatshrink-0.4.1
$(PACKAGES)/$(DETOOLS): $(ML_SOURCES)/$(DETOOLS).tar.gz
	$(MAKE) $(DETOOLS)-all

$(DETOOLS)-all:
	@echo "Building $(DETOOLS)."
	$(MAKE) $(DETOOLS)-unpack
	$(MAKE) $(DETOOLS)-build

$(DETOOLS)-clean:
	cd $(PACKAGES)/$(DETOOLS)

$(ML_SOURCES)/$(DETOOLS).tar.gz:
	mkdir -p $(dir $@)
	wget -O $@ https://github.com/eerimoq/detools/archive/0.45.0.tar.gz

$(DETOOLS)-unpack:
	mkdir -p $(PACKAGES)
	cd $(PACKAGES) && \
	tar xf $(ML_SOURCES)/$(DETOOLS).tar.gz

$(DETOOLS)-build:
	mkdir -p $(SYSROOT)
	cd $(PACKAGES) && \
	touch $(DETOOLS) && \
	cd $(DETOOLS) && \
	$(MAKE) -C src/c library CFLAGS_EXTRA=-I$(SYSROOT)/include && \
	$(MAKE) -C src/c install PREFIX=$(SYSROOT)
