# Copyright (c) 2017 Adys Tech
# Author: Adarsha (mvadu)
CC=g++
CFLAGS= -std=gnu99 -lrt -lm
DEPS = htu21d.h 
INSTALLDIR = /etc/htu21d
SCRIPTDIR = ~/bin/
objects = main.o 

#make predefined:
#$@ = 

#tell make that each .o depends on .c and .h included in DEPS
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
	
# The default rule - compiling our main program:
all:	htu21d
		echo all: make complete
		
htu21d: htu21d.o $(objects)
	$(CC) -o $@ $+ $(CFLAGS)

install: all
		echo Copying files
		test -d $(INSTALLDIR) || mkdir $(INSTALLDIR)			
		cp ./htu21d /etc/htu21d/
		echo Done!!

install_script: all
		echo Copying files
		test -d $(SCRIPTDIR) || mkdir $(SCRIPTDIR)			
		cp ./post_to_influx.sh $(SCRIPTDIR)
		(crontab -l 2>/dev/null; echo "*/5 * * * * ~/bin/post_to_influx.sh living_room htu21d  >/dev/null 2>&1") | sort - | uniq - | crontab -
		echo Done!!

.PHONY: clean
clean: 
	rm *.o
	rm ./htu21d
