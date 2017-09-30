#define HTU21D_Default_Address 0x40  //Unshifted 7-bit I2C address for the sensor

#define Error_I2c_Timeout 	998
#define Error_Bad_Crc		999

#define Trigger_Temp_Measure_Hold  0xE3
#define Trigger_Humd_Measure_Hold  0xE5
#define Trigger_Temp_Measure_Nohold  0xF3
#define Trigger_Humd_Measure_Nohold  0xF5
#define Write_User_Reg  0xE6
#define Read_User_Reg  0xE7
#define Soft_Reset  0xFE

#define Resolution_Mask 0x81
#define Resolution_Rh12_Temp14 0x00
#define Resolution_Rh8_Temp12 0x01
#define Resolution_Rh10_Temp13 0x80
#define Resolution_Rh11_Temp11 0x81

#define End_Of_Battery 0x40
#define Heater_Enabled 0x04
#define Disable_OTP_Reload 0x02


class HTU21D {
    
    public:
        ~HTU21D();
        HTU21D();
        //Public Functions
        bool reset(); 
        float readHumidity();
        float readTemperature();
        float calculateDewPoint(float temp, float humidity);
        float calculateHeatIndex(float temp, float rh);

    private:
        //Private Functions
        bool ValidCyclicRedundancyCheck(uint16_t data, uint8_t crc);
        int msleep(unsigned long milisec);

        //Private Variables
        int _i2cFile;      
};
