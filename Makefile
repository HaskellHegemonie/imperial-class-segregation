CDIR=.
CFILES=$(foreach D,$(CDIR),$(wildcard $(D)/*.c))
COBJ=$(CFILES:.c=.o)
CBIN=$(CFILES:.c=.out)

all:$(CBIN)

%.out:%.o
	$(CC) $(CFLAGS) $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
