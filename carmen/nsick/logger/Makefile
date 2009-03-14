include $(CARMEN_HOME)/src/Makefile.conf

CFLAGS +=
LFLAGS += -lparam_interface -lglobal -lipc -lnanotec_interface -llaser_interface

MODULE_NAME = "NANOTECLOGGER"
MODULE_COMMENT = "Customized module for Nanotec logging."

SOURCES = 

PUBLIC_INCLUDES = 
PUBLIC_LIBRARIES = 
PUBLIC_BINARIES = 

TARGETS = libwritelog.a logger

libwritelog.a: writelog.o

logger: logger.o libwritelog.a

include $(CARMEN_HOME)/src/Makefile.rules
