CFLAGS += -I../drivers/nanotec/vanilla
LDFLAGS += -L ../drivers/nanotec/vanilla -lnanotec

PREFIX = /usr/local

OBJS = nanotec_init nanotec_home nanotec_move nanotec_profile nanotec_nod \
	nanotec_ramp
SRCS = $(OBJ)

all: $(OBJS)

install: $(OBJS)
	install -c -m 755 $(OBJS) $(PREFIX)/bin

clean:
	rm -f *.o $(OBJS)
