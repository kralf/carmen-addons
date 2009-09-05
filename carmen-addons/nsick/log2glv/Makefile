include $(CARMEN_HOME)/src/Makefile.conf

CFLAGS +=
IFLAGS += 
LFLAGS += `gsl-config --libs` -lglv -lglobal -lipc -lz

MODULE_NAME = "LOG2GLV"

SOURCES = 

PUBLIC_INCLUDES = 
PUBLIC_LIBRARIES = 
PUBLIC_BINARIES = 

TARGETS = log2glv

log2glv: transform.o logread.o log2glv.o

include $(CARMEN_HOME)/src/Makefile.rules
