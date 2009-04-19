include $(CARMEN_HOME)/src/Makefile.conf

CFLAGS +=
LFLAGS += -lparam_interface -lglobal -lipc -lepos_interface -llaser_interface

MODULE_NAME = "NSICKLOGGER"
MODULE_COMMENT = "Customized logger module for the nodding SICK"

SOURCES = 

PUBLIC_INCLUDES = 
PUBLIC_LIBRARIES = 
PUBLIC_BINARIES = 

TARGETS = libwritelog.a logger

libwritelog.a: writelog.o

logger: logger.o libwritelog.a

include $(CARMEN_HOME)/src/Makefile.rules
