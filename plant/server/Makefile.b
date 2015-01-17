# Generated automatically from Makefile.in by configure.
# $Id: Makefile.in,v 1.59 2002/03/24 22:20:19 jnelson Exp $

.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: clean mrclean distclean depend all dist

DIR=/root/smart/test/c_environment-master

INCS =  -I$(DIR) \
        -I$(DIR)/hardware \
        -I$(DIR)/hardware/arduino \
        -I$(DIR)/hardware/arduino/cores \
        -I$(DIR)/hardware/arduino/cores/arduino \
        -I$(DIR)/hardware/arduino/variants \
        -I$(DIR)/hardware/arduino/variants/sunxi \
        -I$(DIR)/libraries \
        -I$(DIR)/libraries/Serial \
        -I$(DIR)/libraries/SPI \
	-I$(DIR)/libraries/Wire \
	-I$(DIR)/libraries/LiquidCrystal \
	-I$(DIR)/libraries/PN532_SPI 


GCC_FLAGS = -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wcast-qual\
  -Wtraditional\
  -Wshadow\
  -Wconversion\
  -Waggregate-return\
  -Wmissing-prototypes\
  -Wnested-externs\
  -Wall \
  -Wundef -Wwrite-strings -Wredundant-decls -Winline \
  $(INCS)


srcdir = .
VPATH = .:./../extras
LDFLAGS =  -g
LIBS =  -lsqlite3
CFLAGS = -g -O2 -pipe -Wall -I. $(INCS)

# Change these if necessary

CROSS_PREFIX = arm-linux-
YACC = byacc 
LEX = flex 
CC = arm-linux-gnueabihf-gcc
CPP = arm-linux-gnueabihf-gcc –E

SOURCES = RTC/linker_rtc.c boa/alias.c boa/boa.c boa/buffer.c boa/cgi.c boa/cgi_header.c boa/config.c boa/escape.c \
	thread/scheduleThread.c actions/schedule.c shared/serialport.c boa/actiondispatch.c actions/autosearch.c \
	shared/lightingpack.c shared/dbadapter.c shared/sharemem.c actions/node.c actions/model.c actions/systemSeting.c\
	actions/area.c actions/control.c  actions/home.c boa/action.c\
	boa/get.c boa/hash.c boa/ip.c boa/log.c boa/mmap_cache.c boa/pipe.c boa/queue.c boa/read.c \
	boa/request.c boa/response.c boa/select.c boa/signals.c boa/util.c boa/sublog.c boa/timestamp.c

OBJS = y.tab.o lex.yy.o $(SOURCES:.c=.o) boa/timestamp.o 
%.o: %.cpp
	
all:	smartplant boa_indexer

smartplant:	$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
	
boa_indexer:	boa/index_dir.o boa/escape.o   
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

clean:
	rm -f $(OBJS) smartplant core lex.yy.c y.tab.c y.tab.h *~ boa_indexer boa/index_dir.o boa/*.o
	
distclean:	mrclean

mrclean:	clean
	rm -f config.status config.cache config.h Makefile config.log

# parser dependencies

y.tab.c y.tab.h:	boa/boa_grammar.y
	$(YACC) -d $^

lex.yy.c:	boa/boa_lexer.l
	$(LEX) $^

# timestamp

timestamp.o:	$(SOURCES) boa/boa_grammar.y boa/boa_lexer.l

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
        
# object dump
boa.objdump:    boa
	objdump --disassemble-all --source boa > $@

