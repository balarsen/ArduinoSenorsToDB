#include <Time.h> // https://github.com/PaulStoffregen/Time
#include <Wire.h>
#include <TSL2561.h> // this is the Lux sensor
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC


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
const short TZ_OFFSET = 0;

// how many samples to take and average, more takes longer
// but is more 'smooth'
const short NUMSAMPLES = 5;

//const uint8_t DS3231_ADDR = 0x68;
//const uint8_t DS3231_SELECT_REG = 0x0E;
//const uint8_t DS3231_ESOC_MASK = 0b00011100;

/**********************************
* Global variables
***********************************/
// Timing variables
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned int thermistorValue; // will be from analogRead(A0);

// The address will be different depending on whether you let
// the ADDR pin float (addr 0x39), or tie it to ground or vcc. In those cases
// use TSL2561_ADDR_LOW (0x29) or TSL2561_ADDR_HIGH (0x49) respectively
TSL2561 tsl(TSL2561_ADDR_FLOAT); 

char isodate[25];


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

  Wire.begin();
  if (tsl.begin()) {
    Serial.println("Found Lux sensor");
  } else {
    Serial.println("No Lux sensor?");
  }
  
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
  
//  
//  // clear /EOSC bit
//  // Sometimes necessary to ensure that the clock
//  // keeps running on just battery power. Once set,
//  // it shouldn't need to be reset but it's a good
//  // idea to make sure.
//  Wire.beginTransmission(DS3231_ADDR); // address DS3231
//  Wire.write(DS3231_SELECT_REG); // select register
//  Wire.write(DS3231_ESOC_MASK); // write register bitmap, bit 7 is /EOSC
//  Wire.endTransmission();
//  // now setup to read the time
//  Wire.beginTransmission(0x68); // 0x68 is DS3231 device address
//  Wire.write((byte)0); // start at register 0
//  Wire.endTransmission();
//  Wire.requestFrom(0x68, 3); // request three bytes (seconds, minutes, hours)
//  

    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus() != timeSet) 
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");      


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
  steinhart = inVal / THERMISTORNOMINAL;       // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C
  return steinhart;
}
 
struct Luminosity {
  uint16_t long IR;
  uint16_t long Full;
  uint16_t long Visible;
  uint16_t long Lux;
};

void printDigits(int digits)
{
    // utility function for digital clock display: prints preceding colon and leading 0
    Serial.print(':');
    if(digits < 10)
        Serial.print('0');
    Serial.print(digits);
}

void makeISOdate(){
  // Ask only once for date and time:
  time_t t = now();
  sprintf(isodate, "%4d-%02d-%02dT%02d:%02d:%02dZ",
      year(t), month(t), day(t), hour(t), minute(t), second(t));
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
    float temperatureValue = ThermToTemp( ((float)thermistorValue / (float)NUMSAMPLES) ); 
    Serial.print("Temperature "); 
    Serial.print(temperatureValue);
    Serial.println(" *C");


    Luminosity lum; //  = getLuminosity();
    
    uint32_t tmp = tsl.getFullLuminosity();
    lum.IR = tmp >> 16;
    lum.Full = tmp & 0xFFFF;
    lum.Visible = lum.Full - lum.IR;
    lum.Lux = tsl.calculateLux(lum.Full, lum.IR);
    
    Serial.print("IR: "); Serial.print(lum.IR);   Serial.print("\t\t");
    Serial.print("Full: "); Serial.print(lum.Full);   Serial.print("\t");
    Serial.print("Visible: "); Serial.print(lum.Full - lum.IR);   Serial.print("\t");
  
    Serial.print("Lux: "); Serial.println(lum.Lux);
//


//    Serial.print(hour());
//    printDigits(minute());
//    printDigits(second());
//    Serial.print(' ');
//    Serial.print(day());
//    Serial.print(' ');
//    Serial.print(month());
//    Serial.print(' ');
//    Serial.print(year()); 
//    Serial.println();
makeISOdate();
    Serial.println(isodate);
    
    
//    int seconds = Wire.read(); // get seconds
//    int minutes = Wire.read(); // get minutes
//    int hours = Wire.read();   // get hours
// 
//    seconds = (((seconds & 0b11110000)>>4)*10 + (seconds & 0b00001111)); // convert BCD to decimal
//    minutes = (((minutes & 0b11110000)>>4)*10 + (minutes & 0b00001111)); // convert BCD to decimal
//    hours = (((hours & 0b00100000)>>5)*20 + ((hours & 0b00010000)>>4)*10 + (hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode)
// 
//    Serial.print(hours); Serial.print(":"); Serial.print(minutes); Serial.print(":"); Serial.println(seconds);
// 
  }
}

