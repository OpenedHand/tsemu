LIBS=
INCS=

.c.o:
	$(CC) -g -Wall $(CFLAGS) $(INCS) -c $*.c

OBJS=tsemu.o

tsemu: $(OBJS)
	$(CC) -g -Wall $(CFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJS): input.h  tsemu.h

clean:
	rm -fr *.o tsemu

