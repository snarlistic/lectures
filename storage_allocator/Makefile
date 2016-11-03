# storage_allocator - a general-purpose storage allocator
# Copyright (C) 2016-2016 Freddy Spierenburg
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

# Name of the binary.
NAME=storage_allocator

# Destination where to put our stuff 'make install'.
ifeq ($(origin DESTDIR), undefined)
	bindir=/usr/local/bin
	mandir=/usr/local/man/man1
else
	bindir=$(DESTDIR)/usr/bin
	mandir=$(DESTDIR)/usr/share/man/man1
endif

# List of the objects to built.
OBJECTS=main.o
OBJECTS+=storage_allocator.o
OBJECTS+=debug.o
OBJECTS+=options.o

# Tools 
CLIBS=
CFLAGS=-Wall
CFLAGS+=-DDEBUG=42
LDFLAGS=
CC=gcc
GZIP=gzip
INSTALL=install

.PHONY: all FORCE clean install

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(NAME) $(CLIBS) $(OBJECTS)

clean:
	rm -f $(NAME) $(OBJECTS)

install: $(NAME)
	$(INSTALL) -D -m 0755 $(NAME) $(bindir)/$(NAME)

# Dummy target as dependecy if something has to be build everytime
FORCE:

# Automatic collection of dependencies in the source files.
# It's only updated the first time, after that it must be done maually 
# with "make depend"

# Target to update the file, it's removed first
dependall: dependrm depend

#remove the file
dependrm:
	rm -f dependencies.in
	
# Build the file that contains the dependencies. No deps in this rule.
# If there were deps it would be rebuilt every change, which is unneeded:
depend: dependencies.in 
	@echo "depend"

dependencies.in:
	$(CC) -MM $(CFLAGS) $(patsubst %.o,%.c,$(OBJECTS)) >$@

# The dependecies are included from a separate file:
-include dependencies.in
