GTK_PIDGIN_INCLUDES= `pkg-config --cflags gtk+-2.0 pidgin`

CFLAGS= -O2 -Wall -fpic -g
LDFLAGS= -shared

INCLUDES = \
      $(GTK_PIDGIN_INCLUDES)

whatsapp-pidgin-plugin.so: whatsapp-plugin.c
	gcc whatsapp-plugin.c $(CFLAGS) $(INCLUDES) $(LDFLAGS) -o whatsapp-pidgin-plugin.so

install: whatsapp-pidgin-plugin.so
	mkdir -p ~/.purple/plugins
	cp whatsapp-pidgin-plugin.so ~/.purple/plugins/

uninstall:
	rm -f ~/.purple/plugins/whatsapp-pidgin-plugin.so

clean:
	rm -f whatsapp-pidgin-plugin.so

