CFLAGS += -std=c99 -Wall -Wextra -pedantic -Wold-style-declaration
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
CC     ?= gcc

all: kc

kc: kc.c kc.h Makefile
	$(CC) -O3 $(CFLAGS) -o $@ $< -lX11 $(LDFLAGS)

install: all
	install -Dm755 kc $(DESTDIR)$(BINDIR)/kc
	mkdir -p $(DESTDIR)/usr/share/kc
	cp -r palettes $(DESTDIR)/usr/share/kc

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/kc
	rm -rf $(DESTDIR)/usr/share/kc

clean:
	rm -f kc *.o

.PHONY: all install uninstall clean
