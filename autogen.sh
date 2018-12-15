#!/bin/sh
aclocal
autoheader
glibtoolize --force
automake -a
autoconf
