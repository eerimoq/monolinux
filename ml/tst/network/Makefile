LDFLAGS += -Wl,--wrap=ioctl -Wl,--wrap=socket
INC += ../mocks
SRC += ../mocks/mock.c
SRC += ../mocks/mock_libc.c
COVERAGE_FILTERS = .*ml_network.c

include $(ML_ROOT)/make/suite.mk
