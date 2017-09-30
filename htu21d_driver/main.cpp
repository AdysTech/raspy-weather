#include <stdio.h>
#include <stdint.h> 

#include "HTU21D.h"

int main(int argc, char **argv) {

    HTU21D sensor;
    float t=sensor.readTemperature();
    float rh = sensor.readHumidity();
    float d =  sensor.calculateDewPoint(t,rh);
    float hi = sensor.calculateHeatIndex(t,rh);
    printf("Temperature: %.2f\n", t);
    printf("Relative Humidity: %.2f\n",rh );
    printf("Dew Point: %.2f\n",d);
    printf("Heat Index: %.2f\n",hi);
    return 1;
}