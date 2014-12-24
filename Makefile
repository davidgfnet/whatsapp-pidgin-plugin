GTK_PIDGIN_INCLUDES= `pkg-config --cflags gtk+-2.0 pidgin`

CFLAGS= -O2 -Wall -fpic -g
LDFLAGS= -shared
LIBNAME=whatsapp-pidgin-plugin.so
STRIP=strip
CC=gcc

INCLUDES = \
      $(GTK_PIDGIN_INCLUDES)

.PHONY: all
all: $(LIBNAME)

$(LIBNAME): whatsapp-plugin.c
	$(CC) whatsapp-plugin.c $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o $(LIBNAME)
	$(STRIP) --strip-unneeded $(LIBNAME)

PLUGIN_DIR_PURPLE:=$(shell pkg-config --variable=plugindir purple)
.PHONY: install
install: $(LIBNAME)
	install -D $(LIBNAME) $(DESTDIR)$(PLUGIN_DIR_PURPLE)/$(LIBNAME)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PLUGIN_DIR_PURPLE)/$(LIBNAME)

clean:
	rm -f $(LIBNAME)

