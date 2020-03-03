BUILD = $(shell readlink -f build)
PACKAGES = $(BUILD)/packages

include $(ML_ROOT)/make/packages/packages.mk
