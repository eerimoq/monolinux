ASYNC_RSYNC = $(BUILD)/packages/async/monolinux.rsync
ASYNC_BUILD = $(BUILD)/packages/async/monolinux.build
BITSTREAM_RSYNC = $(BUILD)/packages/bitstream/monolinux.rsync
BITSTREAM_BUILD = $(BUILD)/packages/bitstream/monolinux.build
BUNGA_RSYNC = $(BUILD)/packages/bunga/monolinux.rsync
BUNGA_BUILD = $(BUILD)/packages/bunga/monolinux.build
CURL_RSYNC = $(BUILD)/packages/curl/monolinux.rsync
CURL_BUILD = $(BUILD)/packages/curl/monolinux.build
DBG_MACRO_RSYNC = $(BUILD)/packages/dbg-macro/monolinux.rsync
DBG_MACRO_BUILD = $(BUILD)/packages/dbg-macro/monolinux.build
DETOOLS_RSYNC = $(BUILD)/packages/detools/monolinux.rsync
DETOOLS_BUILD = $(BUILD)/packages/detools/monolinux.build
HEATSHRINK_RSYNC = $(BUILD)/packages/heatshrink/monolinux.rsync
HEATSHRINK_BUILD = $(BUILD)/packages/heatshrink/monolinux.build
HUMANFRIENDLY_RSYNC = $(BUILD)/packages/humanfriendly/monolinux.rsync
HUMANFRIENDLY_BUILD = $(BUILD)/packages/humanfriendly/monolinux.build
MBEDTLS_RSYNC = $(BUILD)/packages/mbedtls/monolinux.rsync
MBEDTLS_BUILD = $(BUILD)/packages/mbedtls/monolinux.build
MESSI_RSYNC = $(BUILD)/packages/messi/monolinux.rsync
MESSI_BUILD = $(BUILD)/packages/messi/monolinux.build
MONOLINUX_C_LIBRARY_RSYNC = $(BUILD)/packages/monolinux-c-library/monolinux.rsync
MONOLINUX_C_LIBRARY_BUILD = $(BUILD)/packages/monolinux-c-library/monolinux.build
PBTOOLS_RSYNC = $(BUILD)/packages/pbtools/monolinux.rsync
PBTOOLS_BUILD = $(BUILD)/packages/pbtools/monolinux.build
XZ_RSYNC = $(BUILD)/packages/xz/monolinux.rsync
XZ_BUILD = $(BUILD)/packages/xz/monolinux.build
ZLIB_RSYNC = $(BUILD)/packages/zlib/monolinux.rsync
ZLIB_BUILD = $(BUILD)/packages/zlib/monolinux.build
LINUX_RSYNC = $(BUILD)/linux/monolinux.rsync
LINUX_BUILD = $(BUILD)/linux/monolinux.build

# Common package definitions.
#
# $1: Package name.
# $2: Package rsync file.
# $3: Package build file.
# $4: Provided static libraries.
#
define PACKAGE_template
packages-rsync: $1-rsync

packages-build: $1-build

$1-all:
	$$(MAKE) $1-rsync
	$$(MAKE) $1-build

$1-build: $3

$1-rsync:
	mkdir -p $$(PACKAGES_DIR)
	if [ -n "$$$$(rsync -ariOu $$(ML_SOURCES)/$1 $$(PACKAGES_DIR))" ] ; then \
	    echo "$1 sources updated." && \
	    touch $2 ; \
	fi

$1-clean:
	rm -rf $$(PACKAGES_DIR)/$1 $4
endef
