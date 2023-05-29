CC=$(CROSS_COMPILE)gcc
CFLAGS += -Wall
CFLAGS += -Werror

ifeq ($(DEBUG),true)
	CFLAGS += -g
endif

default: aesdsocket
all: aesdsocket

aesdsocket: aesdsocket.c 
	$(CC)$(CFLAGS) -o aesdsocket.c aesdsocket

clean:
	rm -rf *.o aesdsocket
