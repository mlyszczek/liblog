LIBS =
SRCS = log.c
MAIN = liblog

SOURCE = -D_POSIX_C_SOURCE=199506L
STD = -std=c99
WARN = -Wall -Wextra -pedantic -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wwrite-strings -Winit-self -Wcast-align -Wstrict-aliasing -Wformat=2 -Wmissing-include-dirs -Wno-unused-parameter -Wuninitialized -Wstrict-overflow=5 -pedantic-errors
CFLAGS = $(STD) $(WARN) $(SOURCE) -fPIC
LDFLAGS = -shared

VERSION = `grep "define VERSION" version.h | cut -d \" -f2`
VERSION_MAJOR = `grep "define VERSION" version.h | cut -d \" -f2 | cut -d. -f1`
CC ?= x86_64-pc-linux-gnu-gcc
BR ?=
INC ?=
LINC ?=
INSTALL_PATH ?=

OBJS = $(SRCS:.c=.o)

.PHONY: depend clean debug release target

all: release

target: CC = $(BR)/output/host/usr/bin/arm-linux-gcc
target: INC = -I$(BR)/output/staging/usr/include
target: LINC = -L$(BR)/output/staging/usr/lib
target: release

release: WARN += -Werror
release: CFLAGS += -O2
release: $(MAIN)

debug: CFLAGS += -O0 -ggdb -g3
debug: $(MAIN)

install: INSTALL_PATH = /usr/local
install: copy_files

target_install: INSTALL_PATH = $(BR)/output/staging/usr
target_install: copy_files

copy_files:
	cp log.h $(INSTALL_PATH)/include
	cp $(MAIN).so.$(VERSION) $(INSTALL_PATH)/lib
	ln -sf $(MAIN).so.$(VERSION) $(INSTALL_PATH)/lib/$(MAIN).so.$(VERSION_MAJOR)
	ln -sf $(MAIN).so.$(VERSION) $(INSTALL_PATH)/lib/$(MAIN).so

clean:
	$(RM) *.o *~ $(MAIN).so.$(VERSION)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(LINC) $(LDFLAGS) -o $(MAIN).so.$(VERSION) $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $(INC) -c $< -o $@
