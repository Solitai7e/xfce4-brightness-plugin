xfce4-brightness-plugin
=======================

(Unfinished) xfce4-panel plugin for controlling software (e.g. RandR)
brightness and gamma levels, to be used with old monitors that do not
dim to a satisfactory degree.

This project will likely remain in its current state until I find the
motivation to work on it again.


Building
--------

The build system is not exactly fully functional at the moment, though
as far as I can tell, the "building" part works.

Ensure the following is installed:

* Autoconf
* Automake
* autoconf-archive
* pkg-config
* GTK 3
* libxfce4panel
* libxfce4utils
* libx11
* libxrandr

`cd` to wherever you cloned this repo to and run:

	./autogen.sh --prefix=/usr
	make -j

A successful build should produce the file `libbrightness.so`.

To install the plugin:

	sudo cp brightness.desktop "$(pkg-config --variable=datarootdir libxfce4panel-2.0)/xfce4/panel/plugins/"
	sudo cp libbrightness.so "$(pkg-config --variable=libdir libxfce4panel-2.0)/xfce4/panel/plugins/"


To-dos
------

In case if I ever feel like coming back to this:

* Per-monitor brightness/gamma adjustment
* Fix the build system (I hate build systems)
* Investigate Wayland support
