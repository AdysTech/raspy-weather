# Copyright (c) 2015 Adys Tech
# Author: Adarsha (mvadu)
CC=gcc
CFLAGS= -std=gnu99 -lrt -lm
DEPS = pi_2_mmio.h common_dht_read.h pi_2_dht_read.h 
INSTALLDIR = /etc/dht11
objects = pi_2_mmio.o common_dht_read.o pi_2_dht_read.o 

#make predefined:
#$@ = 

#tell make that each .o depends on .c and .h included in DEPS
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
# The default rule - compiling our main program:
all:	dht11
		echo all: make complete
		
dht11: dht11.o $(objects)
	$(CC) -o $@ $+ $(CFLAGS)

install: all
		echo Copying files
		test -d $(INSTALLDIR) || mkdir $(INSTALLDIR)			
		cp ./dht11 /etc/dht11/
		echo Done!!

.PHONY: clean
clean: 
	rm *.o
