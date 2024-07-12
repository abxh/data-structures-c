TOPTARGETS := all clean test
SUBDIRS := $(wildcard ./examples/*) $(wildcard ./tests/*) $(wildcard ./test_benchmark/*)

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)
