xbobar
======

tl;dr: it's a desktop panel for your X session.

xbobar is a Javascript + HTML runtime for building desktop panels. At
its core, it's a QtWebKit widget, providing a simple API to its hosted
page for simple things such as reading files, network access and DBus.
The JS runtime provides a CommonJS environment, which includes high
level abstractions over the basic API. There is also RxJS, React and
JQuery.

With all this, you can build any kind of desktop panel you've dreamed
of, using only your favouritest web technologies, because everyone
loves the DOM.

# Incomplete

This is absolutely not anywhere close to being complete and usable.
Pull requests welcome.

# Building

You'll need Qt5 and libdbus installed and working. Substitute for your
preferred platform:

```sh
$ pacman -S qt5-base qt5-webkit libdbus
```

To build the binary and JSDoc:

```sh
$ qmake
$ make
$ make jsdoc
```

# Running

From your checkout:

```sh
$ ./build/xbobar --debug default.js
```

# License

Copyright 2014 Bodil Stokke

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see
[http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).
