/*
------------ FishTank -------------
https://github.com/jeromelacote/FishTank
By mail@jeromelacote.com - more at www.thenewburo.com
and check www.victorise.com

This script:

- reads temperature
- stores temperature on a SD CARD
- trigger a relay (heater) if the temperature is within a range

 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
 * 

Some random links for documentation and examples
https://create.arduino.cc/projecthub/TheGadgetBoy/ds18b20-digital-temperature-sensor-and-arduino-9cc806
http://playground.arduino.cc/Code/dateTime
https://randomnerdtutorials.com/guide-to-sd-card-module-with-arduino/
https://howtomechatronics.com/tutorials/arduino/control-high-voltage-devices-arduino-relay-tutorial/
https://github.com/sparkfun/IoT_Power_Relay
https://www.google.com/shopping/product/15196433662715787558?q=16%C3%972+LCD+Display+%26+Keypad+Shield&biw=1621&bih=756&prds=paur:ClkAsKraXxaA_92k1NMnRzUaoUh-Rs-MS_jptxp1gVrzPWOEAT6y1bCZI6bFEghy1N47alL_3OgeRA8AxHUREFSu8NUqHjRAx3Jlym14dC-9Ot6uoPyKPASBGxIZAFPVH72Fy3ArGnrw2__ggRLNWtNr4VFTgA&sa=X&ved=0ahUKEwjRuf_8vtvgAhVOsp4KHZU4BtMQ8wIIywI
 */
 
/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
#include <Math.h>
/********************************************************************/

#define ONE_WIRE_BUS  2     // Data wire is plugged into pin 2 on the Arduino 
#define btnRIGHT      0
#define btnUP         1
#define btnDOWN       2
#define btnLEFT       3
#define btnSELECT     4
#define btnNONE       5

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices  
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 
File myFile;                  //Instanciate a file to write on card
// select the pins used on the LCD panel
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7); // legacy code if the led is used as a shield, we changed that to accomodate the Pin 4 used by the SD card reader
LiquidCrystal lcd(9, 10, 3, 5, 7, 8);

int lcd_key             =  0;
int adc_key_in          =  0;
const int buttonPin     =  5;          // the number of the pushbutton pin
const int relayPin      =  6;         // the number of the relay pin
float tempMax           = 77;        // the temperature maximum (in celsius), the relay will turn off
float tempMin           = 78;       // the temperature minimum (in celsius), the releay will turn on
const int interval      = 2;       // how often the data is writen on the SdCard (in minutes)
int buttonState         = 0;      // variable for reading the pushbutton status
float tempBuffer        = 1;
float tempIncrement     = 1;
bool isHeaterRequired   = true;
float curTemp           = 0;
int cnt                 = 0;
String curTime          = "";
 //this is a list of int variables used in this clock program
int s = 0;
int sec = 0;
int hrs = 0;
int minutes = 0;
int initialHours = 02;//variable to initiate hours
int initialMins = 0;//variable to initiate minutes
int initialSecs = 00;//variable to initiate seconds

/////////////////////////////////////////////////////////////////////////
/**************************** S E T U P ********************************/ 
/////////////////////////////////////////////////////////////////////////

void setup() {
  
 Serial.begin(9600);            // Open serial communications and wait for port to open:
 pinMode(relayPin, OUTPUT);     // pinMode(buttonPin, INPUT); 
 lcd.begin(16, 2);              // start the library for LCD
 lcd.setCursor(0,0);            // start to write on lcs 1st col - 1st row
 lcd.print("Start!");           // print a welcome message 
 sensors.begin(); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
   writeSD("LOG.TXT", "- Start Device - Time Reset");
}

///////////////////////////////////////////////////////////////////////
/**************************** L O O P ********************************/ 
///////////////////////////////////////////////////////////////////////

void loop() {

  cnt++; // just a counter, it's our clock

   //Time management 
  long seconds = millis() / 1000;
  long minutes = seconds / 60;
  long hours = minutes / 60;
  long days = hours / 24;
  curTime = String(days) + ":" +  String(hours % 24) + ":" +  String(minutes % 60) + ":" +  String(seconds % 60); 

 lcd.setCursor(12,0);            // move cursor to second line "1" and 9 spaces over
 lcd.print(millis()/1000);
 lcd.setCursor(15,1);  
 
 if( (cnt) %2 == 0){  //Display a little animation to show the controller heart beat!
  lcd.print("#");
  }else{
    lcd.print("_");
   }

 lcd.print(millis()/1000); //  display the clock
 
//check buttons
 lcd_key = read_LCD_buttons();  // read the buttons
  switch (lcd_key)               // depending on which button was pushed, we perform an action
   {
     case btnRIGHT:
       {
        tempBuffer+=tempIncrement;
        Serial.print("btnRIGHT");
       break;
       }
     case btnLEFT:
       {
        tempBuffer-=tempIncrement;
        Serial.print("btnLEFT");
       break;
       }
     case btnUP:
       {
        Serial.print("btnUP");
        tempMin+=tempIncrement;
       break;
       }
     case btnDOWN:
       {
        Serial.print("btnDOWN");
        tempMin-=tempIncrement;
       break;
       }
     case btnSELECT:
       {
        Serial.print("btnSELECT");
       break;
       }
   }
   
   Serial.print("--"+String(tempBuffer));
  if(tempBuffer<0){tempBuffer =1;}
  tempMax = tempMin +tempBuffer;
  lcd.setCursor(0,1); 
  int tempMinDisplay = round(tempMin);
  int tempMaxDisplay = round(tempMax);
  lcd.print("Min:"+String(tempMinDisplay)+" Max:"+String(tempMaxDisplay)); //display the min and max temp

 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  curTemp = celciusToFarenheit(sensors.getTempCByIndex(0)); // Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
   
 if(curTemp<tempMin ){
    isHeaterRequired =true;
  }
   if(curTemp>tempMax ){
    isHeaterRequired =false;
  }
  if(isHeaterRequired == true){ // trigger the relay!
    //Heater is Required
    digitalWrite(relayPin, HIGH);
  }else{
     //Heater is not Required
     digitalWrite(relayPin, LOW);
  }

  //the display on the screen
  lcd.setCursor(0,0); 
  lcd.print("         "); //clear
  lcd.setCursor(0,0); 
  lcd.print(String(curTemp)+"F"); //display the temp
  Serial.print(curTime+"\t"+curTemp+"\n");

  if (cnt > (interval*60)){  // Write on the SD CArd every Interval
   Serial.print("\n Temperature is: \t"+String(curTemp)+"- is Heater Required? "+ String(isHeaterRequired)+"\n"); 
   writeSD("LOG.TXT", "time:\t"+curTime+"\t Temperature is: \t"+String(curTemp)+"\tF - \t"+ String(farenheittoCelsisu(curTemp))+ "\tC\t" );
   cnt = 0;
  }
   delay(500); // wait on bit so we don't stress the cpu too much
} 

//////////////////////////////////////////////////////////////
void writeSD(String filename, String val )
{
  myFile = SD.open(filename, FILE_WRITE);
  // if the file opened okay, write to it:
  if (myFile) {
    myFile.println(val);
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening"+filename);
  }
 }
/////////////////////////////////////////////////////////
float celciusToFarenheit(float celsius)
{
  float f = (celsius * 1.8) + 32;
  return f;  
}
/////////////////////////////////////////////////////////
float farenheittoCelsisu(float f)
{
  float c = (f - 32) *1.8;
  return c;  
}
/////////////////////////////////////////////////////////
// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 if (adc_key_in < 50)   return btnRIGHT; 
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;  
 return btnNONE;  // when all others fail, return this...
}
/////////////////////////////////////////////////////////
//this method is for seconds
int seconds()
{
    s = initialHours*3600;
    s = s+(initialMins*60);
    s = s+initialSecs;
    s = s+(millis()/1000);
    return s;
}
/////////////////////////////////////////////////////////
//this method is for hours
int hours()
{
    hrs = seconds();
    hrs = hrs/3600;
    hrs = hrs%24;
    return hrs;
}
/////////////////////////////////////////////////////////
//this method is for minutes
int mins()
{
    minutes = seconds();
    minutes = minutes/60;
    minutes = minutes%60;
    return minutes;
}
/////////////////////////////////////////////////////////
int secs()
{
    sec = seconds();
    sec = sec%60;
    return sec;
}
/////////////////////////////////////////////////////////
String printDigits(byte digits){
    if(digits < 10)
        return('0'+String(digits));
    return(String(digits));
}
/////////////////////////////////////////////////////////


// Example code to read file on sdcard
// re-open the file for reading:
//  myFile = SD.open("LOG.TXT");
//  if (myFile) {
//    Serial.println("LOG.TXT exist");
//
//    // read from the file until there's nothing else in it:
//    while (myFile.available()) {
//      Serial.write(myFile.read());
//    }
//    // close the file:
//    myFile.close();
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening LOG.TXT");
//  }
