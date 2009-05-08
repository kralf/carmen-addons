include $(CARMEN_HOME)/src/Makefile.conf

CFLAGS += -I../../../drivers/vanilla/epos -I../../../drivers/vanilla/can \
	-I../../../posix/config -I../../../posix/thread
LFLAGS += -lparam_interface -lglobal -lipc -lepos -lthread

MODULE_NAME = "NSICKEPOS"
MODULE_COMMENT = "Module for the EPOS nodding SICK control interface"

SOURCES = 

PUBLIC_INCLUDES = epos_messages.h epos_interface.h
PUBLIC_LIBRARIES = libepos_interface.a
PUBLIC_BINARIES = epos

TARGETS = libepos_interface.a epos

libepos_interface.a: epos_interface.o

epos: epos_ipc.o epos.o

include $(CARMEN_HOME)/src/Makefile.rules
