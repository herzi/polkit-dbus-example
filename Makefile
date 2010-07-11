all: mirror-server mirror-client

mirror-server: mirror-server.c mirror-server-glue.h
	gcc -g -Wall $(shell pkg-config --cflags --libs dbus-glib-1 polkit-gobject-1) $< -o $@

mirror-server-glue.h: mirror.xml Makefile
	dbus-binding-tool --mode=glib-server --prefix=mirror $< > $@

mirror-client: mirror-client.c mirror-client-glue.h
	gcc -g -Wall $(shell pkg-config --cflags --libs gtk+-2.0 dbus-glib-1) $< -o $@

mirror-client-glue.h: mirror.xml Makefile
	dbus-binding-tool --mode=glib-client --prefix=mirror $< > $@

install:
	install -m644 mirror.conf $(DESTDIR)/etc/dbus-1/system.d
	install -m644 mirror.policy $(DESTDIR)/usr/share/polkit-1/actions
