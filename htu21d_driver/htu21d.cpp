// Copyright (c) 2017 Adys Tech
// Author: Adarsha (mvadu)
// License : MIT License
// ref : http://www.te.com/commerce/DocumentDelivery/DDEController?Action=srchrtrv&DocNm=HPC199_6&DocType=Data+Sheet&DocLang=English
//       https://github.com/sparkfun/HTU21D_Breakout/blob/master/Libraries/Arduino/src/SparkFunHTU21D.cpp
//       https://github.com/ms-iot/htu21d/blob/develop/HTU21D.cs
// Uses native i2c headers, enable i2c in raspi-config, and reboot. 

#include <stdio.h>
#include <stdint.h> 
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
//#include <linux/i2c.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <math.h>

#include "htu21d.h"

HTU21D::HTU21D() {
    _i2cFile = -1;
}

HTU21D::~HTU21D() {
    if(_i2cFile == -1)
        close(_i2cFile);
}

bool HTU21D::reset() {

    _i2cFile = open("/dev/i2c-1", O_RDWR);

    if (_i2cFile < 0) {
            fprintf(stderr,"Error opening file: %s\n", strerror(errno));
            return false;
    }

    if (ioctl(_i2cFile, I2C_SLAVE, HTU21D_Default_Address) < 0) {
            fprintf(stderr,"ioctl failed: %s\n", strerror(errno));
            return false;
    }

    char cmd = Soft_Reset;
    int i = write(_i2cFile, &cmd, 1);
    if (i != 1) {
            fprintf(stderr,"i2c write transaction failed: %s\n", strerror(errno));
    }

    msleep(20);
    // Read the status register to check it's really there
    cmd = Read_User_Reg;
    i = write(_i2cFile, &cmd, 1);
    if (i != 1) {
            fprintf(stderr,"i2c write transaction failed: %s\n", strerror(errno));
    }
    
    i = read(_i2cFile, &cmd, 1);
    if (i != 1) {
            fprintf(stderr,"i2c write transaction failed: %s\n", strerror(errno));
    }

    //default value of User Reg = 0x02
    return (cmd == 0x02);
}

float HTU21D::readHumidity() {
    if(_i2cFile == -1 && !reset()) {
        fprintf(stderr,"Unable to initialize HTU21D\n");
    }
        
    uint8_t cmd = Trigger_Humd_Measure_Hold;
    int i = write(_i2cFile, &cmd, 1);
    if (i != 1) {
            fprintf(stderr,"i2c write transaction failed: %s\n", strerror(errno));
    }

    msleep(60);

    uint8_t rbuf[3];
    i = read(_i2cFile, rbuf, 3);
    if (i != 3)
    {
        fprintf(stderr,"i2c read transaction failed: %s\n", strerror(errno));
    }

    uint16_t rawValue = (((uint16_t) rbuf[0] << 8) | (uint16_t) rbuf[1]) & 0xFFFC;
    
    //Given the raw humidity data, calculate the actual relative humidity
    if(ValidCyclicRedundancyCheck(rawValue, rbuf[2] ^ 0x62))
        return  (rawValue * (125.0 / 65536.0) - 6.0); 
    return Error_Bad_Crc;
}

float HTU21D::readTemperature() {
    if(_i2cFile == -1 && !reset()) {
        fprintf(stderr,"Unable to initialize HTU21D\n");
    }

    uint8_t cmd = Trigger_Temp_Measure_Nohold;
    int i = write(_i2cFile, &cmd, 1);
    if (i != 1) {
            fprintf(stderr,"i2c write transaction failed: %s\n", strerror(errno));
    }

    msleep(60);

    uint8_t rbuf[3];
    i = read(_i2cFile, rbuf, 3);
    if (i != 3)
    {
        fprintf(stderr,"i2c read transaction failed: %s\n", strerror(errno));
    }

    // Reconstruct the result using the first two bytes returned from the device
    // NOTE: Zero out the status bits (bits 0 and 1 of the LSB), but keep them in place
    // - status bit 0 - not assigned
    // - status bit 1
    // -- off = temperature data
    // -- on = humdity data    
    uint16_t rawValue = (((uint16_t) rbuf[0] << 8) | (uint16_t) rbuf[1]) & 0xFFFC;

    if(ValidCyclicRedundancyCheck(rawValue, rbuf[2]))
        return  (rawValue * 175.72 / 65536 - 46.85);
    return Error_Bad_Crc;
}

int HTU21D::msleep(unsigned long milisec) {
    struct timespec req={0};
    time_t sec=(int)(milisec/1000);
    milisec=milisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=milisec*1000000L;
    while(nanosleep(&req,&req)==-1)
        continue;
    return 1;
}

bool HTU21D::ValidCyclicRedundancyCheck(uint16_t data, uint8_t crc) {
    // Validate the 8-bit cyclic redundancy check (CRC) byte
    // CRC: http://en.wikipedia.org/wiki/Cyclic_redundancy_check
    // Generator polynomial x^8 + x^5 + x^4 + 1: 100110001(0x0131)
    const int CrcBitLength = 8;
    const int DataLength = 16;
    const uint16_t GeneratorPolynomial = 0x0131;
       
    // input bit first padded with zeroes corresponding to the bit length n of the CRC
    uint32_t crcData = data << CrcBitLength;

    for (int i = DataLength - 1; 0 <= i; --i) {
        if (0 ==  (0x01 & (crcData >> (CrcBitLength + i))))
            continue;
        crcData ^= GeneratorPolynomial << i;
    }

    return (crc == crcData);
}

float HTU21D::calculateDewPoint(float temp, float humid) {
    const float DewConstA = 8.1332;
    const float DewConstB = 1762.39;
    const float DewConstC = 235.66;
    
    float paritalPressure;
    float dewPoint;

    // To calculate the dew point, the partial pressure must be determined first.
    // See datasheet page 16 for details.
    // Partial pressure = 10 ^ (A - (B / (Temp + C)))
    paritalPressure = DewConstA - (DewConstB / (temp + DewConstC));
    paritalPressure = pow(10, paritalPressure);

    // Dew point is calculated using the partial pressure, humidity and temperature.
    // The datasheet says "Ambient humidity in %RH, computed from HTU21D(F) sensor" on page 16 is doesn't say to use the temperature compensated
    // RH value. Therefore, we use the raw RH value straight from the sensor.
    // Dew point = -(C + B / (log(RH * PartialPress / 100) - A))
    dewPoint = humid * paritalPressure / 100;
    dewPoint = log10 (dewPoint) - DewConstA;
    dewPoint = DewConstB / dewPoint;
    dewPoint = -(dewPoint + DewConstC);

    return (dewPoint);

}

//ref: http://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml

float HTU21D::calculateHeatIndex(float temp, float rh) {
        //convert temp to fahrehneit  
        float T = 9.0/5.0 * temp + 32;
        
        //this simple formula is computed first, if it's 80 degrees F or higher, the full regression equation along with any adjustment is applied. 
        float heatIndex = 0.5 * (T + 61.0 + ((T-68.0)*1.2) + (rh*0.094));

        if(heatIndex > 80) {
            float heatIndex = -42.379 + 2.04901523*T + 10.14333127*rh - .22475541*T*rh 
                              - .00683783*T*T - .05481717*rh*rh + .00122874*T*T*rh 
                              + .00085282*T*rh*rh - .00000199*T*T*rh*rh;
            if(rh < 13 && T > 80 && T < 112){
                heatIndex -= ((13-rh)/4)*sqrt((17-(T>95.0?T-95.0:95.0-T))/17);
            } else if(rh>85 && T>80 && T<87) {
                heatIndex += ((rh-85)/10) * ((87-T)/5); 
            }
        }
        //convert back to celsius
        return (5.0/9.0 * (heatIndex -32));
}    