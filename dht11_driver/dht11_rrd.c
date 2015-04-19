//
//  How to access GPIO registers from C-code on the Raspberry-Pi
//  Example program
//  15-January-2012
//  Dom and Gert
//  Revised: 15-Feb-2013


// Access from ARM Running Linux

#define GPIO_BASE_OFFSET 0x200000
#define GPIO_LENGTH 4096


#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <unistd.h>

#include "pi_2_mmio.h"
#include "pi_2_dht_read.h"

#define POWER_PIN 23
#define SIGNAL_PIN 24
#define RRD_FILE "dht11.rrd"


void power_on_dht11()
{
	pi_2_mmio_set_output(POWER_PIN);
	pi_2_mmio_set_high(POWER_PIN);

	pi_2_mmio_set_input(SIGNAL_PIN);

	//sleep_milliseconds(200);
	volatile int t = 0;
	while (!pi_2_mmio_input(SIGNAL_PIN))
	{
		t++;
		if (t > 200) break;
	}
	if (t > 200)
		//singal pin should have a pullup, so on powerup, it should be high
		//if(!pi_2_mmio_input(SIGNAL_PIN))
	{
		printf("DHT11 was not detected!! \nMake sure the sensor is connected and there is a 4.7k pullup between +ve and signal pins.\n");
		pi_2_mmio_set_low(POWER_PIN);
		exit(-1);
	}
	return;
}

int main(int argc, char **argv)
{

	// Set up gpi pointer for direct register access
	int r = pi_2_mmio_init();
	if (r != MMIO_SUCCESS)
	{
		printf("Error initiating Mem GPIO %d", r);
		exit(-1);
	}

	power_on_dht11();

	float humidity = 0, temperature = 0;
	int result = pi_2_dht_read(SIGNAL_PIN, &humidity, &temperature);

	//poweroff sensor
	pi_2_mmio_set_low(POWER_PIN);
	pi_2_mmio_set_low(SIGNAL_PIN);

	if (temperature == 0 || humidity == 0)
	{
		printf("Error getting data from DHT11");
		exit(-1);
	}

	double dP = dewPointFast(temperature, humidity);
	//double hI = humidex(temperature, dP);

	char cCurrentPath[FILENAME_MAX];
	char *exe = argv[0];
	char *ssc;
	int l = 0;
	ssc = strstr(exe, "/");
	do{
		l = strlen(ssc) + 1;
		exe = &exe[strlen(exe) - l + 2];
		ssc = strstr(exe, "/");
	} while (ssc);
	ssc = strstr(argv[0], exe);

	strncpy(cCurrentPath, argv[0], ssc - argv[0]);
	cCurrentPath[ssc - argv[0]] = '\0';

	FILE *stdin = NULL;
	int status;
	char str[FILENAME_MAX + 300];

	sprintf(str, "%s%s", cCurrentPath, RRD_FILE);

	if (access(str, F_OK) == -1)
	{
		// file doesn't exist, create RRD tool db
		//RRA1 - 5 min 12 samples - 1 hour
		//RRA2 - 15 min 96 samples - 1 day
		//RRA3 - 30 min 1440 samples - 1 month
		//RRA4&5 - 1440 min 365 samples - 1 year min/max
		
		sprintf(str, "rrdtool create %s%s \
 					--start now \
					--step 60  \
					DS:temp:GAUGE:360:0:60  \
					DS:humidity:GAUGE:360:10:100  \
					DS:dewpoint:GAUGE:360:-60:100 \
					RRA:LAST:0.5:1:1  \
					RRA:AVERAGE:0.5:5:12  \
					RRA:AVERAGE:0.5:15:96  \
					RRA:AVERAGE:0.5:30:1440  \
					RRA:MIN:0.5:1440:365  \
					RRA:MAX:0.5:1440:365", cCurrentPath, RRD_FILE);
		stdin = popen(str, "w");
		printf("RRD created");
		if (stdin != NULL)
		{
			status = pclose(stdin);
			if (status == -1)
			{
				printf("Error reported by pclose()");
				exit(-1);
			}
		}
	}

	stdin = NULL;

	sprintf(str, " rrdtool update %s%s N:%f:%f:%f", cCurrentPath, RRD_FILE, temperature, humidity,dP);
	printf(str);
	stdin = popen(str, "r");
	if (stdin != NULL)
	{
		status = pclose(stdin);
		if (status == -1)
		{
			printf("Error reported by pclose()");
			exit(-1);
		}
	}

	printf("RRD Updated");
	return 0;

} // main