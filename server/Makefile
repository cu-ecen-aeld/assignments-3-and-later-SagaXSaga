CC=$(CROSS_COMPILE)gcc
CFLAGS += -Wall
CFLAGS += -Werror

ifeq ($(DEBUG),true)
	CFLAGS += -g
endif

default: aesdsocket
all: aesdsocket

aesdsocket: 
	$(CC)$(CFLAGS) aesdsocket.c -o aesdsocket

clean:
	rm -rf *.o aesdsocket
