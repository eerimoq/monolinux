LDFLAGS += -Wl,--wrap=ioctl -Wl,--wrap=socket
INC += ../mocks
SRC += ../mocks/mock.c
SRC += ../mocks/mock_libc.c

include $(ML_ROOT)/make/suite.mk
