all: mirror-server mirror-client

mirror-server: mirror-server.c mirror-server-glue.h
	gcc -g -Wall $(shell pkg-config --cflags --libs dbus-glib-1 polkit-gobject-1) $< -o $@

mirror-server-glue.h: mirror.xml Makefile
	dbus-binding-tool --mode=glib-server --prefix=mirror $< > $@

mirror-client: mirror-client.c mirror-client-glue.h
	gcc -g -Wall $(shell pkg-config --cflags --libs gtk+-2.0 dbus-glib-1 polkit-gtk-1) $< -o $@

mirror-client-glue.h: mirror.xml Makefile
	dbus-binding-tool --mode=glib-client --prefix=mirror $< > $@

install:
	mkdir -p $(DESTDIR)/usr/bin
	install -m755 mirror-client $(DESTDIR)/usr/bin
	install -m755 mirror-server $(DESTDIR)/usr/bin
	mkdir -p $(DESTDIR)/etc/dbus-1/system.d
	install -m644 mirror.conf $(DESTDIR)/etc/dbus-1/system.d
	mkdir -p $(DESTDIR)/usr/share/polkit-1/actions
	install -m644 mirror.policy $(DESTDIR)/usr/share/polkit-1/actions

clean:
	rm -rf mirror-server mirror-client mirror-server-glue.h mirror-client-glue.h

dist:
	mkdir -p polkit-example-0.0.1
	cp Makefile mirror-server.c mirror-client.c mirror.h mirror.xml mirror.conf mirror.policy polkit-example-0.0.1
	tar czf polkit-example-0.0.1.tar.gz polkit-example-0.0.1
	rm -rf polkit-example-0.0.1
