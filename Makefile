#
# Makefile
#
# Targets:
#   all
# 		The default target, if not target is specified. Compiles source files
#		as necessary and links them into the final executable.
#   clean
#		Removes all object files and executables.
#   install
#		Copies the library and header files to the directory specified by the
#		prefix variable. Defaults to /usr/local.
#	uninstall
#		Removes the library and header files from the directory specified by the
#		prefix variable. Defaults to /usr/local.
# Variables:
#   CC
#      	The C compiler to use. Defaults to gcc.
#   CFLAGS
#      	Flags to pass to the C compiler. Defaults to -I/usr/include.
#   debug=1
#       Build with debug info
#   LDFLAGS
#      	Flags to pass to the linker. Defaults to -L/usr/lib.
#   prefix
#      	The directory to install copt.h and libcopt.a into. Defaults to 
#		/usr/local.
#

ifeq ($(CC),)
CC=gcc
endif
CFGLAGS += -I/usr/include
LDFLAGS += -L/usr/lib -Tcopt.ld
ifdef debug
CFLAGS += -ggdb -D DEBUG
LDFLAGS += -v
else
CFLAGS += -O3 -flto
LDFLAGS += -flto
endif
ifeq ($(prefix),)
prefix = /usr/local
endif

OBJS = copt.o

.PHONY: all clean install uninstall

all: clean install test uninstall

test: main.o libcopt.a
	$(CC) $(LDFLAGS) $^ -o $@
ifndef debug
	strip $@
endif

libcopt.a: $(OBJS) copt.h
	ar cr $@ $(OBJS)

%.o: %.c copt.h
	$(CC) $(CFLAGS) -c $< -o $@

install:
	cp copt.h $(prefix)/include
	cp libcopt.a $(prefix)/lib

uninstall:
	- rm -f $(prefix)/include/copt.h
	- rm -f $(prefix)/lib/libcopt.a

clean:
	- rm -f test
	- rm -f *.o
	- rm -f *.a
