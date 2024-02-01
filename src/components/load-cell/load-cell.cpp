#include <HX711.h> //import the HX711 library for using the load cel
#define LOADCELL_DOUT_PIN 21 // Define the DOUT pin of the load cell. This is connected to GPIO 21
#define LOADCELL_SCK_PIN 22 // Define the SCK pin of the load cell. This is connected to GPIO 22

// Create an instance of the HX711 library per the HX711 library documentation
HX711 scale; // we can call member functions of this object to interact with the load cell (ie 'scale.get_units()' to
             // get the weight in grams)
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
double read_scale()
{
        scale.power_up(); // power up the ADC (HX711) per the HX711 library documentation
        Serial.print("scale reading:\t");
        double weight = scale.get_units(); // get the weight in grams
        Serial.println(weight, 1); // get the weight in grams and print it to the serial monitor. the 1 indicates
                                 // how many decimal places to print
        scale.power_down(); // put the ADC (HX711) in sleep mode after we are done reading the weight. this saves power
        return weight;

}