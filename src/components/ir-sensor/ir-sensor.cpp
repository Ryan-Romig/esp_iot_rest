#include <Arduino.h>


#define IR_SENSOR 13 // Define the IR sensor pin. This is connected to GPIO 13
#define SENSOR_POWER_GPIO 19 // Define the GPIO pin to provide power to \
        the IR sensor. Setting this pin provide 3.3v at 40mA max and can be \
        used for power




// create function to setup the IR sensor. run this in setup()
void setup_ir_sensor()
{
    pinMode(IR_SENSOR, INPUT); // Set the IR sensor GPIO pin to input mode (ie read mode)
    pinMode(SENSOR_POWER_GPIO, OUTPUT); // set sensor power gpio pin to output mode
    digitalWrite(SENSOR_POWER_GPIO, HIGH); // Set GPIO 34 high to provide 3.3V
}
// create function to read the IR sensor. run this in loop()
void read_ir_sensor()
{
        int sensorValue = digitalRead(IR_SENSOR);// 0 is on, 1 is off
        Serial.print("IR Sensor"); // Read the input from GPIO 13
        Serial.println(sensorValue); // Print the value to the serial monitor
}