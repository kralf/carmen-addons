CFLAGS += -I../nanotec/vanilla
LDFLAGS += -L ../nanotec/vanilla -lnanotec

PREFIX = /usr/local

OBJS = nanotec_init nanotec_home nanotec_move nanotec_profile nanotec_nod
SRCS = $(OBJ)

all: $(OBJS)

install: $(OBJS)
	install -c -m 755 $(OBJS) $(PREFIX)/bin

clean:
	rm -f *.o $(OBJS)
