CFLAGS += -std=c99 -Wall -Wextra -pedantic -Wold-style-declaration
PREFIX ?= /usr
BINDIR ?= $(PREFIX)/bin
CC     ?= gcc

all: kc

kc: kc.c kc.h Makefile
	$(CC) -O3 $(CFLAGS) -o $@ $< -lX11 $(LDFLAGS)

install: all
	install -Dm755 kc $(DESTDIR)$(BINDIR)/kc
	mkdir -p $(DESTDIR)/etc/kc
	cp -r palettes $(DESTDIR)/etc/kc

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/kc
	rm -rf $(DESTDIR)/etc/kc

clean:
	rm -f kc *.o

.PHONY: all install uninstall clean
