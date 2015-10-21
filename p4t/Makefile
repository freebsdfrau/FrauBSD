MKDIR = mkdir
INSTALL = install -C
DESTDIR = /usr/local

.PHONY: all install

all:

install:
	$(INSTALL) p4t $(DESTDIR)/bin/p4t
	$(MKDIR) -p $(DESTDIR)/libexec/p4t
	$(INSTALL) -v libexec/* $(DESTDIR)/libexec/p4t
