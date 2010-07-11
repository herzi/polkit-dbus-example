all: server client

server:
	dbus-binding-tool --mode=glib-server --prefix=mirror mirror.xml > mirror-server-glue.h
	gcc -g -Wall `pkg-config --cflags --libs dbus-glib-1 polkit-gobject-1` mirror-server.c -o mirror-server

client:
	dbus-binding-tool --mode=glib-client --prefix=mirror mirror.xml > mirror-client-glue.h
	gcc -g -Wall `pkg-config --cflags --libs gtk+-2.0 dbus-glib-1 polkit-dbus` mirror-client.c -o mirror-client

install:
	sudo cp mirror.conf /etc/dbus-1/system.d
	sudo cp mirror.policy /usr/share/polkit-1/actions
