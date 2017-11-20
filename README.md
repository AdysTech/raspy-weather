# Raspberry-Pi-Indoor-Weather-Station
A set of C libraries to gather Temperature and Humidity data from various temperature/humidity sensors. Sensor packages are coded to run in user mode, without sudo. Tested on Raspbian Stretch.

### Sensors covered

#### DHT11
The DHT11 is a basic, ultra low-cost digital temperature and humidity sensor. Its fairly simple to use, but requires careful timing to grab data. Part of the integration with DHT11 code  (pi_2_dht_read.c) is based on [Adafruit_Python_DHT](https://github.com/adafruit/Adafruit_Python_DHT/) which was Modified to turn on the sensor only when needed (via GPIO pin), and using the calibration pulses built into the DHT11 communication protocol.
The library will power up the sensor (it will detect if the sensor is not connected!), read the temperature and humidity values. 

#### HTU21D
The HTU21D is a I2C digital Temperature / humidity sensor with a typical accuracy of ±2% with an operating range that's optimized from 5% to 95% RH, temperature output has an accuracy of ±1°C from -30~90°C.
THe code uses Linux core I2C libraries, and does not depend on any external packages (like wiringPi).