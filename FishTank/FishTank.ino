/*
---- FishTank ----
This script:

- read temperature
- store temperature on a SD CARD
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



 */
 

/********************************************************************/
// First we include the libraries
#include <OneWire.h> 
#include <DallasTemperature.h>
#include <SPI.h>
#include <SD.h>
#include <LiquidCrystal.h>
/********************************************************************/

#define ONE_WIRE_BUS 2     // Data wire is plugged into pin 2 on the Arduino 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices  
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature. 
File myFile;                  //Instanciate a file to write on card
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

int lcd_key     = 0;
int adc_key_in  = 0;
const int buttonPin = 5;     // the number of the pushbutton pin
const int relayPin =  6;     // the number of the relay pin
int tempMax = 28;      // the temperature maximum (in celsius), the relay will turn off
int tempMin = 24;      // the temperature minimum (in celsius), the releay will turn on
const int interval = 100;    // how often the data is writen on the SdCard
int buttonState = 0;         // variable for reading the pushbutton status
bool isHeaterRequired = true;
float curTemp = 0;
int cnt = 0;

 //thhis is a list of int variables used in this clock program
int s=0;
int sec=0;
int hrs=0;
int minutes=0;
int initialHours = 02;//variable to initiate hours
int initialMins = 0;//variable to initiate minutes
int initialSecs = 00;//variable to initiate seconds


/**************************** S E T U P ********************************/ 
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
 // initialize the pushbutton pin as an input:
 // pinMode(buttonPin, INPUT);
//  pinMode(relayPin, OUTPUT);
  
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);
 lcd.print("Start!"); // print a simple messag
 
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

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
writeSD("LOG.TXT", "- Start Device -");

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
  
}

/**************************** L O O P ********************************/ 
void loop() {

  cnt++; // just a counter, it's our clock

 lcd.setCursor(12,0);            // move cursor to second line "1" and 9 spaces over
 lcd.print(millis()/1000);
 
//check buttons
 lcd_key = read_LCD_buttons();  // read the buttons
switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
      Serial.print("btnRIGHT");
     break;
     }
   case btnLEFT:
     {
      Serial.print("btnLEFT");
      tempMin--;
     break;
     }
   case btnUP:
     {
      Serial.print("btnUP");
      tempMin++;
     break;
     }
   case btnDOWN:
     {
      Serial.print("btnDOWN");

     break;
     }
   case btnSELECT:
     {
      Serial.print("btnSELECT");
     break;
     }

 }
 tempMax = tempMin +1;
lcd.setCursor(0,1); 
lcd.print("Max:"+String(tempMax)+" Min:"+String(tempMin));
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
 sensors.requestTemperatures(); // Send the command to get temperature readings 
/********************************************************************/
curTemp = sensors.getTempCByIndex(0); // Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
 if(curTemp<tempMin ){
    isHeaterRequired =true;
  }
   if(curTemp>tempMax ){
    isHeaterRequired =false;
  }
  if(isHeaterRequired == true){
    //Heater is Required
    digitalWrite(relayPin, HIGH);
  }else{
     //Heater is not Required
     digitalWrite(relayPin, LOW);
  }
 //Serial.print(" Temperature is: "+String(curTemp)+"- is Heater Required? "+ String(isHeaterRequired)+"\n"); 

lcd.setCursor(0,0); 
lcd.print(String(curTemp));

  // read the state of the pushbutton value:
//  buttonState = digitalRead(buttonPin);
//
//  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
//  if (buttonState == HIGH) {
//  // turn LED on:
// // Serial.print(" Button s on"); 
//  digitalWrite(relayPin, HIGH);
//  } else {
//  //Serial.print(" Button s oFFFFFFF"); 
//  digitalWrite(relayPin, LOW);
//  
//  }
  
//if (cnt>interval){
//  writeSD("LOG.TXT", " Temperature is: "+String(curTemp));
//  cnt = 0;
//  }

 Serial.print(
    printDigits(hours())
    +":"+
    printDigits(mins())
    +":"+
    printDigits(secs())
    +"\r\n"
    );
   delay(500); 
} 

//////////////////////////////////////////////////////////////


void writeSD(String filename, String val ){
  
  
  myFile = SD.open(filename, FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Starting to write Log...");
    myFile.println(val);
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening"+filename);
  }
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


//this method is for seconds
int seconds()
{
    s = initialHours*3600;
    s = s+(initialMins*60);
    s = s+initialSecs;
    s = s+(millis()/1000);
    return s;
}
//this method is for hours
int hours()
{
    hrs = seconds();
    hrs = hrs/3600;
    hrs = hrs%24;
    return hrs;
}
//this method is for minutes
int mins()
{
    minutes = seconds();
    minutes = minutes/60;
    minutes = minutes%60;
    return minutes;
}
 
int secs()
{
    sec = seconds();
    sec = sec%60;
    return sec;
}

String printDigits(byte digits){
    if(digits < 10)
        return('0'+String(digits));
    return(String(digits));
}
