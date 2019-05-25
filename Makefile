SUITES := $(shell ls tst)

.PHONY: all clean

all:
	for test in $(SUITES) ; do \
	  echo ; \
	  echo "============================ $$test ============================" ; \
	  $(MAKE) -C tst/$$test ; \
	done

clean:
	for test in $(SUITES) ; do \
	  $(MAKE) -C tst/$$test clean ; \
	done
