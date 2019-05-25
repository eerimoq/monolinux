INC += $(BUILD)/libcurl/include
LDFLAGS += -L$(BUILD)/libcurl/lib -lcurl

default:
	$(MAKE) $(BUILD)/libcurl
	$(MAKE) build

include $(ML_ROOT)/make/app.mk

$(BUILD)/libcurl: $(ML_SOURCES)/curl-7.65.0.tar.xz
	mkdir -p $(BUILD)
	cd $(BUILD) && \
	tar xf $(ML_SOURCES)/curl-7.65.0.tar.xz && \
	cd curl-7.65.0 && \
	./configure CFLAGS="-ffunction-sections -fdata-sections" \
	    --prefix=$(BUILD)/libcurl \
	    --without-ssl --without-zlib --disable-shared \
	    --disable-ftp --disable-ldap --disable-telnet --disable-dict \
	    --disable-file --disable-tftp --disable-imap --disable-pop3 \
	    --disable-smtp --disable-rtsp --disable-gopher && \
	$(MAKE) && \
	$(MAKE) install

#
# Configuration for ARM.
#

# $(BUILD)/libcurl: $(ML_SOURCES)/curl-7.65.0.tar.xz
# 	mkdir -p $(BUILD)
# 	cd $(BUILD) && \
# 	tar xf $(ML_SOURCES)/curl-7.65.0.tar.xz && \
# 	export CROSS_COMPILE=arm-linux-musleabi && \
# 	export AR=$$CROSS_COMPILE-ar && \
# 	export AS=$$CROSS_COMPILE-as && \
# 	export LD=$$CROSS_COMPILE-ld && \
# 	export RANLIB=$$CROSS_COMPILE-ranlib && \
# 	export CC=$$CROSS_COMPILE-gcc && \
# 	export NM=$$CROSS_COMPILE-nm && \
# 	cd curl-7.65.0 && \
# 	./configure CFLAGS="-ffunction-sections -fdata-sections" \
# 	    --prefix=$(BUILD)/libcurl --target=$$CROSS_COMPILE \
# 	    --host=$$CROSS_COMPILE --build=i586-pc-linux-gnu \
# 	    --without-ssl --without-zlib --disable-shared \
# 	    --disable-ftp --disable-ldap --disable-telnet --disable-dict \
# 	    --disable-file --disable-tftp --disable-imap --disable-pop3 \
# 	    --disable-smtp --disable-rtsp --disable-gopher && \
# 	$(MAKE) && \
# 	$(MAKE) install
