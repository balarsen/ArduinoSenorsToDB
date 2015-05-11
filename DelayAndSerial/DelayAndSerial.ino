/* Based on Blink without Delay, David A. Mellis, Paul Stoffregen, Scott Fitzgerald

*/



/**********************************
* Setting up constants
***********************************/
// constants won't change. Used here to set a pin number :
const int LEDPIN =  13;      // the number of the LED pin
const long BLINKINTERVAL = 1000;           // interval at which to blink (milliseconds)
const long THERMISTORPIN = A0; 
const long SERIALSPEED = 9600;

// how many samples to take and average, more takes longer
// but is more 'smooth'
const long NUMSAMPLES = 5;



/**********************************
* Global variables
***********************************/
// Timing variables
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned int thermistorValue; // will be from analogRead(A0);
float temperatureValue;

void setup() {
  // set the onboard digital pin as output:
  pinMode(THERMISTORPIN, INPUT);    
  pinMode(LEDPIN, OUTPUT);    
  digitalWrite(LEDPIN, LOW);
  Serial.begin(SERIALSPEED); 
  while (!Serial) {
    ; // wait for serial port to connect. Likely not needed
  }
  Serial.println("Starting up!"); 


}

float CtoF(float inC) {
  /**********************************
  * CtoF(float inC)
  * Converts degrees C to degrees F
  * inC - float value of degrees C
  * return is the temperature in F
  ***********************************/
  return ((inC * 1.8) + 32);  // 1.8 is 9/5
}

float ThermToTemp(float inVal) {
  /**********************************
  * ThermToTemp(float inVal)
  * Function is for an adafruit 372 thermistor
  * inVal - float value as read from the ADC, float because one may want to average the values before conversion
  * return is the temperature in C
  ***********************************/
  // thermistor equation defines  (http://en.wikipedia.org/wiki/Thermistor#B_or_.CE.B2_parameter_equation)
  // From https://learn.adafruit.com/thermistor/using-a-thermistor
  // resistance at 25 degrees C
  const long THERMISTORNOMINAL = 10000;    
  // temp. for nominal resistance (almost always 25 C)
  const long TEMPERATURENOMINAL = 25;  
  // The beta coefficient of the thermistor (usually 3000-4000)
  const long BCOEFFICIENT = 3950;
  // the value of the 'other' resistor
  const long SERIESRESISTOR = 10020; // measured

  float steinhart;
  inVal = 1023 / inVal - 1;
  inVal = SERIESRESISTOR / inVal;
  steinhart = inVal / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  return steinhart;
}
 




void loop()
{
  uint8_t i;

  // check to see if it's time to blink the LED; that is, if the
  // difference between the current time and last time you blinked
  // the LED is bigger than the interval at which you want to
  // blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= BLINKINTERVAL) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // set the LED with the ledState of the variable:
    digitalWrite(LEDPIN, digitalRead(LEDPIN)^1);
    
    // take N samples in a row, with a slight delay
    thermistorValue = 0;
    for (i=0; i < NUMSAMPLES; i++) {
      thermistorValue += analogRead(THERMISTORPIN);
      delay(10);
    }
    temperatureValue = ThermToTemp( ((float)thermistorValue / (float)NUMSAMPLES) ); 
    Serial.print("Temperature "); 
    Serial.print(temperatureValue);
    Serial.println(" *C");


  }
}

