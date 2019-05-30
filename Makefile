.PHONY: test clean

test:
	$(MAKE) -C ml test

run:
	$(MAKE) -C ml run

clean:
	$(MAKE) -C ml clean
