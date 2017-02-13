LIBS =
SRCS = log.c
MAIN = liblog

SOURCE = -D_POSIX_C_SOURCE=199506L
STD = -std=c99
WARN = -Wall -Wextra -pedantic -pedantic-errors
CFLAGS = $(STD) $(WARN) $(SOURCE) -fPIC
LDFLAGS = -shared

VERSION = `grep "define VERSION" version.h | cut -d \" -f2`
VERSION_MAJOR = `grep "define VERSION" version.h | cut -d \" -f2 | cut -d. -f1`
DESTDIR ?= `cat .destdir`
CC ?=
INC ?=
LINC ?=

OBJS = $(SRCS:.c=.o)

.PHONY: depend clean debug release target

all: release

release: WARN += -Werror
release: CFLAGS += -O2
release: $(MAIN)

debug: CFLAGS += -O0 -ggdb -g3
debug: $(MAIN)

install:
	install -m 0644 -D -t $(DESTDIR)/include log.h
	install -m 0755 -D -t $(DESTDIR)/lib     $(MAIN).so.$(VERSION)
	ln -sf $(MAIN).so.$(VERSION) $(DESTDIR)/lib/$(MAIN).so.$(VERSION_MAJOR)
	ln -sf $(MAIN).so.$(VERSION) $(DESTDIR)/lib/$(MAIN).so
	echo $(DESTDIR) > .destdir

uninstall:
	$(RM) $(DESTDIR)/include/log.h
	$(RM) $(DESTDIR)/lib/$(MAIN).so.$(VERSION)
	@if [ ! -f $(DESTDIR)/lib/$(MAIN).so.$(VERSION_MAJOR) ]; then \
	    $(RM) $(DESTDIR)/lib/$(MAIN).so.$(VERSION_MAJOR); \
	fi
	@if [ ! -f $(DESTDIR)/lib/$(MAIN).so ]; then \
	    $(RM) $(DESTDIR)/lib/$(MAIN).so; \
	fi

clean:
	$(RM) *.o *~ $(MAIN).so.$(VERSION)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(LINC) $(LDFLAGS) -o $(MAIN).so.$(VERSION) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
