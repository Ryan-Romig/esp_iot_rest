#include <Arduino.h> // Include the Arduino library for using most things
#include <HX711.h> //import the HX711 library for using the load cel

#define LOADCELL_DOUT_PIN 21 // Define the DOUT pin of the load cell. This is connected to GPIO 21
#define LOADCELL_SCK_PIN 22 // Define the SCK pin of the load cell. This is connected to GPIO 22

// Create an instance of the HX711 library per the HX711 library documentation
HX711 scale; // we can call member functions of this object to interact with the load cell (ie 'scale.get_units()' to
             // get the weight in grams)

#define IR_SENSOR 13 // Define the IR sensor pin. This is connected to GPIO 13
#define SENSOR_POWER_GPIO                                                                                              \
    34 // Define the GPIO pin to provide power to the IR sensor. Setting this pin provide 3.3v at 40mA max and can be
       // used for power

// create function to setup the local cell (scale). run this in setup()
void setup_scale()
{
    Serial.println("Initializing the scale"); // print to the serial monitor
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // call the begin function to activate the load cell functionality
    scale.set_scale(
        10); // this value is obtained by calibrating the scale with known weights; see the README for details
    scale.tare(); // reset the scale to 0 (tare)
}
// create function to read the scale. run this in loop()
void scale_loop()
{
    scale.power_up(); // power up the ADC (HX711) per the HX711 library documentation
    Serial.print("one reading:\t");
    double weight = scale.get_units(); // get the weight in grams
    Serial.print(weight, 1); // get the weight in grams and print it to the serial monitor. the 1 indicates
                                        // how many decimal places to print
    scale.power_down(); // put the ADC (HX711) in sleep mode after we are done reading the weight. this saves power
    delay(1000); // wait one second before taking another reading. this delay blocks the loop from running too fast
}

// create function to setup the IR sensor. run this in setup()
void setup_ir_sensor()
{
    pinMode(IR_SENSOR, INPUT); // Set the IR sensor GPIO pin to input mode (ie read mode)
    pinMode(SENSOR_POWER_GPIO, OUTPUT); // set sensor power gpio pin to output mode
    digitalWrite(SENSOR_POWER_GPIO, HIGH); // Set GPIO 34 high to provide 3.3V
}
// create function to read the IR sensor. run this in loop()
void ir_sensor_loop()
{
    int sensorValue = digitalRead(IR_SENSOR); // Read the input from GPIO 13
    Serial.println(sensorValue); // Print the value to the serial monitor
}

// the setup function runs once when you press reset or power the board
void setup()
{
    Serial.begin(115200); // Start the serial monitor at 115200 baud for debugging
    setup_ir_sensor();//run the ir sensor setup function 
    setup_scale(); // run the scale setup function
}
// main code that runs over and over after the setup is complete
void loop()
{
    scale_loop(); // run the scale loop function
    ir_sensor_loop(); // run the ir sensor loop function
}
