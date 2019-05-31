LDFLAGS += -Wl,--wrap=ioctl
LDFLAGS += -Wl,--wrap=socket
LDFLAGS += -Wl,--wrap=ml_close
LDFLAGS += -Wl,--wrap=ml_shell_register_command
LDFLAGS += -Wl,--wrap=sendto
INC += ..
SRC += ../utils/mocks/mock.c
SRC += ../utils/mocks/mock_libc.c
SRC += ../utils/mocks/mock_ml_shell.c
SRC += ../utils/utils.c
COVERAGE_FILTERS = .*ml_network.c

include $(ML_ROOT)/make/suite.mk
