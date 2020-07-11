CFLAGS += -std=c99 -Wall -Wextra -pedantic -Wold-style-declaration
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
CC     ?= gcc

all: kfc

kfc: kfc.c kfc.h Makefile
	$(CC) -O3 $(CFLAGS) -o $@ $< -lX11 $(LDFLAGS)

install: all
	install -Dm755 kfc $(DESTDIR)$(BINDIR)/kfc
	mkdir -p $(DESTDIR)/usr/share/kfc
	cp -r palettes $(DESTDIR)/usr/share/kfc

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/kfc
	rm -rf $(DESTDIR)/usr/share/kfc

clean:
	rm -f kfc *.o

.PHONY: all install uninstall clean
