# see Config file for compile-time settings
include Config

MAKEFLAGS += --no-print-directory


# default: build all targets enabled in Config
all:
	@echo $(LIBRARY)
	@cd src; $(MAKE) clean $(LIBRARY)
ifneq ($(BUILD_UTILITIES),0)
	@cd utils; $(MAKE) clean all
endif
ifneq ($(BUILD_TESTING),0)
	@cd tests; $(MAKE) clean all
endif


# run basic regression tests
test:
	@cd tests; $(MAKE) test


# clean all
clean:
	@cd src; $(MAKE) clean
	@cd utils; $(MAKE) clean
	@cd tests; $(MAKE) clean
