# Raspberry-Pi-Indoor-Weather-Station
A set of C libraries to gather Temperature and Humidity data from generic DHT11 sensor, store it in a round-robin database and show it in a PHP page.

Part of the integration with DHT11 code  (pi_2_dht_read.c) is based on [Adafruit_Python_DHT](https://github.com/adafruit/Adafruit_Python_DHT/) which was Modified to turn on the sensor only when needed (via GPIO pin), and using the calibration pulses built into the DHT11 communication protocol.

The library will power up the sensor (it will detect if the sensor is not connected!), read the temperature and humidity values. It will then store the values into a rrdtool based round robin database with enough values to get a trend for whole year.

A cron job executes the library everyminute, and the PHP gets the data using `rrdtool lastupdate` method.

My current version can be accessed at http://raspberrypi.adystech.com/sysinfo.php
