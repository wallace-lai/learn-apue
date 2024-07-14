CFLAGS+=-pthread
LDFLAGS+=-pthread

all:relayer

relayer: main.o relayer_fsm.o relayer_init.o relayer.o
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf *.o relayer