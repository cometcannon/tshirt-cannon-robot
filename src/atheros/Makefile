CC := @gcc

SIMPLE_UI_OBJS = simple_ui.o
ATHEROS_INTERFACE_OBJS = atheros_interface.o

APPS = simple_ui atheros_interface

all: $(APPS)

simple_ui: $(SIMPLE_UI_OBJS)
	$(CC) -o $@ $(SIMPLE_UI_OBJS)

atheros_interface: $(ATHEROS_INTERFACE_OBJS)
	$(CC) -o $@ $(ATHEROS_INTERFACE_OBJS)

clean:
	@rm -f *.o $(APPS) *~
