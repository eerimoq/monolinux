# A package.

SUITES ?= $(shell ls tst)

.PHONY: test clean

test:
	for test in $(SUITES) ; do \
	  echo ; \
	  echo "============================ $$test ============================" ; \
	  $(MAKE) -C tst/$$test || exit 1 ; \
	done

clean:
	for test in $(SUITES) ; do \
	  $(MAKE) -C tst/$$test clean ; \
	done

print-%:
	@echo $($*)
