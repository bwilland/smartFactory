# Generated automatically from Makefile.in by configure.
# $Id: Makefile.in,v 1.59 2002/03/24 22:20:19 jnelson Exp $

.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: clean mrclean distclean depend all dist



GCC_FLAGS = -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wcast-qual\
  -Wtraditional\
  -Wshadow\
  -Wconversion\
  -Waggregate-return\
  -Wmissing-prototypes\
  -Wnested-externs\
  -Wall \
  -Wundef -Wwrite-strings -Wredundant-decls -Winline


srcdir = .
VPATH = .:./../extras
LDFLAGS =  -g
LIBS =  -lsqlite3 -I/usr/include/libxml2/ -lxml2
CFLAGS = -g -O2 -pipe -Wall -I.

# Change these if necessary

YACC = byacc 
LEX = flex 
CC = gcc 
CPP = gcc -E

SOURCES = actions/node.c thread/scheduleThread.c shared/serialport.c actions/autosearch.c \
	shared/lightingpack.c shared/dbadapter.c shared/sharemem.c thread/alarmThread.c \
	shared/socket.c smart.c thread/xmlParse.c thread/cmdTransThread.c thread/heartBeatThread.c

OBJS = $(SOURCES:.c=.o) 

all:	smartfactory 

smartfactory:	$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
	
boa_indexer:	boa/index_dir.o boa/escape.o   
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) smartfactory core 
	
distclean:	mrclean

mrclean:	clean
	rm -f config.status config.cache config.h Makefile config.log

# parser dependencies

# depend stuff
.depend:
	$(CPP) -MM $(SOURCES) > .depend
        
depend:
	-rm -f .depend
	$(MAKE) .depend
        
include .depend

# tags
tags:	$(SOURCES)
	ctags -o tags $^ *.h

# dist
dist:
	$(MAKE) clean
	./makedist.sh
        
