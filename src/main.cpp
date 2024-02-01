#include <Arduino.h> // Include the Arduino library for using most things
#include "components/load-cell/load-cell.hpp" // Include the load cell header file
#include "components/ir-sensor/ir-sensor.hpp" // Include the ir sensor header file


// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(115200); // Start the serial monitor at 115200 baud for debugging
    setup_ir_sensor(); // run the ir sensor setup function
    setup_scale(); // run the scale setup function
}
// main code that runs over and over after the setup is complete
void loop()
{
    read_scale(); // run the scale loop function
    read_ir_sensor(); // run the ir sensor loop function
    delay(1000); // wait 1000ms before looping. delaying a 'restart'  just a hair helps stability slightly
}
