include $(CARMEN_HOME)/src/Makefile.conf

CFLAGS +=
LFLAGS += -lparam_interface -lglobal -lipc -lnanotec

MODULE_NAME = "NANOTECSERVER"
MODULE_COMMENT = "Server for the Nanotec pan unit"

SOURCES = 

PUBLIC_INCLUDES = nanotec_messages.h nanotec_interface.h
PUBLIC_LIBRARIES = libnanotec_interface.a
PUBLIC_BINARIES = nanotec

TARGETS = libnanotec_interface.a nanotec

libnanotec_interface.a:	nanotec_interface.o

nanotec: nanotec_ipc.o nanotec.o

include $(CARMEN_HOME)/src/Makefile.rules
