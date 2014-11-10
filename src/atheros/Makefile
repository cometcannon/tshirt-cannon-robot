CC := @gcc

SIMPLE_UI_OBJS = simple_ui.o

APPS = simple_ui

all: $(APPS)

simple_ui: $(SIMPLE_UI_OBJS)
	$(CC) -o $@ $(SIMPLE_UI_OBJS)

clean:
	@rm -f *.o $(APPS) *~
