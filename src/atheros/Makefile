CFLAGS = -std=gnu99 -Wall -Wno-unused-variable -Wno-unused-parameter
CC := @gcc

SIMPLE_UI_OBJS = simple_ui.o
ATHEROSD_OBJS = atherosd.o

APPS = simple_ui atherosd

all: $(APPS)

simple_ui: $(SIMPLE_UI_OBJS)
	$(CC) -o $@ $(SIMPLE_UI_OBJS) $(CFLAGS)

atherosd: $(ATHEROSD_OBJS)
	$(CC) -o $@ $(ATHEROSD_OBJS) $(CFLAGS) -lpthread

clean:
	@rm -f *.o $(APPS) *~
