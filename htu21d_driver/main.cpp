#include <stdio.h>
#include <string.h> 
#include <stdint.h>

#include "HTU21D.h"

#define ToFahrehneit(temp) (9.0/5.0 * temp + 32)

int showHelp() {
    printf ("Usage\n htu21d : gives Temperature and Humidity in metric (Centigrade) usints\n -f : Converts temperature to Fahrehneit\n -h or /? : print this help\n");
    return 0;
} 

int main(int argc, char **argv) {

    bool fahrehneit = false;
    
    if(argc == 2) {
		if(!strcmp(argv[1], "-f"))
            fahrehneit = true;
        if(!strcmp(argv[1], "/?") || !strcmp(argv[1], "-h"))
			return showHelp();
	}
    

    HTU21D sensor;
    float t=sensor.readTemperature();
    float rh = sensor.readHumidity();
    float d =  sensor.calculateDewPoint(t,rh);
    float hi = sensor.calculateHeatIndex(t,rh);
    if(fahrehneit) {
        t = ToFahrehneit(t);
        d = ToFahrehneit (d);
        hi = ToFahrehneit(hi);
    }
    printf("Temperature: %.2f\n", t);
    printf("Relative Humidity: %.2f\n",rh );
    printf("Dew Point: %.2f\n",d);
    printf("Heat Index: %.2f\n",hi);
    return 1;
}