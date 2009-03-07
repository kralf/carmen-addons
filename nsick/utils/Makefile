CFLAGS += -I../nanotec/vanilla
LDFLAGS += -L ../nanotec/vanilla -lnanotec

PREFIX = /usr/local

OBJS = nanotec_home
SRCS = $(OBJ)

all: $(OBJS)

install: $(OBJS)
	install -c -m 755 $(OBJS) $(PREFIX)/bin

clean:
	rm -f *.o $(OBJS)
