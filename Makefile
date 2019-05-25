.PHONY: test clean

test:
	$(MAKE) -C ml test

clean:
	$(MAKE) -C ml clean
