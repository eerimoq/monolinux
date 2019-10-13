INC += $(ML_ROOT)/ml/include $(BUILD)/root/include
SRC += \
	main.c \
	$(ML_ROOT)/ml/src/ml.c \
	$(ML_ROOT)/ml/src/ml_bus.c \
	$(ML_ROOT)/ml/src/ml_dhcp_client.c \
	$(ML_ROOT)/ml/src/ml_inet.c \
	$(ML_ROOT)/ml/src/ml_libc.c \
	$(ML_ROOT)/ml/src/ml_log_object.c \
	$(ML_ROOT)/ml/src/ml_message.c \
	$(ML_ROOT)/ml/src/ml_network.c \
	$(ML_ROOT)/ml/src/ml_queue.c \
	$(ML_ROOT)/ml/src/ml_shell.c \
	$(ML_ROOT)/ml/src/ml_timer.c \
	$(ML_ROOT)/ml/src/ml_worker_pool.c
OBJ = $(patsubst %,$(BUILD)%,$(abspath $(SRC:%.c=%.o)))
CFLAGS += $(INC:%=-I%)
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections -L$(BUILD)/root/lib $(LIBS:%=-l%)
STRIP ?= no
DEPSDIR = $(BUILD)/deps

.PHONY: clean

app: $(EXE)

$(EXE): $(OBJ)
	@echo "LD $@"
	mkdir -p $(BUILD)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -lpthread -o $@
ifeq ($(STRIP), yes)
	$(CROSS_COMPILE)strip $@
endif

define COMPILE_template
-include $(patsubst %.c,$(DEPSDIR)%.o.dep,$(abspath $1))
$(patsubst %.c,$(BUILD)%.o,$(abspath $1)): $1
	@echo "CC $1"
	mkdir -p $(BUILD)$(abspath $(dir $1))
	mkdir -p $(DEPSDIR)$(abspath $(dir $1))
	$$(CC) $$(CFLAGS) -c -o $$@ $$<
	gcc -MM -MT $$@ $$(CFLAGS) -o $(patsubst %.c,$(DEPSDIR)%.o.dep,$(abspath $1)) $$<
endef
$(foreach file,$(SRC),$(eval $(call COMPILE_template,$(file))))

clean:
	rm -rf $(BUILD) $(CLEAN)

print-%:
	@echo $($*)
