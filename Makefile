CFLAGS=-Wall
CDIR=.
LIBDIR=./lib

CFILES=$(foreach D,$(CDIR),$(wildcard $(D)/*.c))
COBJ=$(CFILES:.c=.o)
CBIN=$(CFILES:.c=.out)

LHEAD=$(foreach L,$(LIBDIR),$(wildcard $(L)/*.h))
LSRC=$(LHEAD:.h=.c)
LOBJ=$(LSRC:.c=.o)
LBIN=$(LSRC:.c=.out)

all:$(LOBJ) $(CBIN)

%.out:%.o
	$(CC) $(CFLAGS) $(LOBJ) $< -o $@

%.o:%.c
	$(CC) $(CFLAGS) -c $< -o $@
