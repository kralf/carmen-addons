CFLAGS += -fPIC
LDFLAGS += -lm

PREFIX = /usr/local

libnanotec.so: global.h global.c util.h util.c nanotec.h nanotec.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o libnanotec.so \
	global.c util.c nanotec.c

install: libnanotec.so
	install -c -m 644 libnanotec.so $(PREFIX)/lib

clean:
	rm -f *.o *.a *.so
