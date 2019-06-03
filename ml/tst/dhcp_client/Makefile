LDFLAGS += -Wl,--wrap=socket
LDFLAGS += -Wl,--wrap=ml_read
LDFLAGS += -Wl,--wrap=ml_write
LDFLAGS += -Wl,--wrap=ml_close
LDFLAGS += -Wl,--wrap=timerfd_create
LDFLAGS += -Wl,--wrap=timerfd_settime
LDFLAGS += -Wl,--wrap=poll
INC += ..
SRC += ../utils/mocks/mock.c
SRC += ../utils/mocks/mock_libc.c

include $(ML_ROOT)/make/suite.mk
