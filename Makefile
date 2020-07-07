CFLAGS += -std=c99 -Wall -Wextra -pedantic -Wold-style-declaration
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
CC     ?= gcc

all: cpal

cpal: cpal.c cpal.h Makefile
	$(CC) -O3 $(CFLAGS) -o $@ $< -lX11 $(LDFLAGS)

install: all
	install -Dm755 cpal $(DESTDIR)$(BINDIR)/cpal
	mkdir -p $(DESTDIR)/etc/okpal
	cp -r palettes $(DESTDIR)/etc/okpal

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/cpal
	rm -rf $(DESTDIR)/etc/okpal

clean:
	rm -f cpal *.o

.PHONY: all install uninstall clean
