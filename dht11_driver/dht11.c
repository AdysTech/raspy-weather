// Copyright (c) 2015 Adys Tech
// Author: Adarsha (mvadu)

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
#include <math.h>

#include "pi_2_mmio.h"
#include "pi_2_dht_read.h"

#define POWER_PIN 23
#define SIGNAL_PIN 24


void power_on_dht11()
{
	pi_2_mmio_set_output(POWER_PIN);
	pi_2_mmio_set_high(POWER_PIN);

	pi_2_mmio_set_input(SIGNAL_PIN);

	//sleep_milliseconds(200);
	volatile int t = 0;
	//wait for GPIO to change and DHT11 to power on. Signal pin on DHT11 should have a pullup, so on powerup, it should be high
	while (!pi_2_mmio_input(SIGNAL_PIN))
	{
		t++;
		if (t > 200) break;
	}
	if (t > 200)		
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
	//wait for a sec before using the sensor
	sleep(1);
	
	float humidity = 0, temperature = 0;
	int result = pi_2_dht_read(SIGNAL_PIN, &humidity, &temperature);


	double dP = dewPointFast(temperature, humidity);
	double hI = humidex(temperature, dP);
	
	printf("DHT11 out\tTemp:\t%f\tHumidity:\t%f\tDP:\t%f\tHI:\t%f\n", temperature, humidity,dP,hI );

	//poweroff sensor
	pi_2_mmio_set_low(POWER_PIN);
	pi_2_mmio_set_low(SIGNAL_PIN);
	return 0;

} // main

