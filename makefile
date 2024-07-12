TOPTARGETS := all clean test
SUBDIRS := $(wildcard ./examples/*) $(wildcard ./tests/*)

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
