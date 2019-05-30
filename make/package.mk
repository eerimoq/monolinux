# A package.

SUITES ?= $(filter-out mocks,$(shell ls tst))

.PHONY: test clean coverage

test:
	for test in $(SUITES) ; do \
	  echo ; \
	  echo "============================ $$test ============================" ; \
	  $(MAKE) -C tst/$$test || exit 1 ; \
	done
	$(MAKE) coverage

coverage:
	gcovr --exclude tst --html-details --output index.html
	mkdir -p build/coverage
	mv index.* build/coverage

clean:
	for test in $(SUITES) ; do \
	  $(MAKE) -C tst/$$test clean ; \
	done
	rm -rf build

print-%:
	@echo $($*)
