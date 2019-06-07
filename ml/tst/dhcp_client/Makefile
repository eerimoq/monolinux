LDFLAGS += -Wl,--wrap=socket
LDFLAGS += -Wl,--wrap=bind
LDFLAGS += -Wl,--wrap=setsockopt
LDFLAGS += -Wl,--wrap=sendto
LDFLAGS += -Wl,--wrap=ml_read
LDFLAGS += -Wl,--wrap=ml_close
LDFLAGS += -Wl,--wrap=timerfd_create
LDFLAGS += -Wl,--wrap=timerfd_settime
LDFLAGS += -Wl,--wrap=poll
LDFLAGS += -Wl,--wrap=ml_network_interface_index
LDFLAGS += -Wl,--wrap=ml_network_interface_configure
LDFLAGS += -Wl,--wrap=ml_network_interface_mac_address
LDFLAGS += -Wl,--wrap=ml_network_interface_add_route
INC += ..
SRC += ../utils/mocks/mock.c
SRC += ../utils/mocks/mock_libc.c
SRC += ../utils/mocks/mock_ml_network.c

include $(ML_ROOT)/make/suite.mk
