#include <stdio.h>
#include <string.h> 
#include <stdint.h>

#include "htu21d.h"

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
    const char* unit = (fahrehneit?"Fahrehneit":"Celsius");
    printf("Temperature\t%.2f\t%s\n", t,unit);
    printf("Relative Humidity\t%.2f\tPercentage\n",rh );
    printf("Dew Point\t%.2f\t%s\n",d,unit);
    printf("Heat Index\t%.2f\t%s\n",hi,unit);
    return 1;
}