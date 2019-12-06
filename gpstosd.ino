#include <SPI.h>
#include <SD.h>

#include <SoftwareSerial.h>
#include <TinyGPS.h>

SoftwareSerial mySerial(5,6);
TinyGPS gps;

void gpsdump(TinyGPS &gps);
void printFloat(double f, int digits = 2);

File myFile;

void setup() {
  // put your setup code here, to run once:
  // Oploen serial communications and wait for port to open:
  Serial.begin(9600);
  // set the data rate for the SoftwareSerial port
  mySerial.begin(9600);
  delay(1000);

  Serial.print("Sizeof(gpsobject) = "); 
  Serial.println(sizeof(TinyGPS));
  Serial.println(); 

  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
 
}

void loop() {
  // put your main code here, to run repeatedly:
 bool newdata = false;
  unsigned long start = millis();
  // Every 5 seconds we print an update
  while (millis() - start < 5000) 
  {
    if (mySerial.available()) 
    
    {
      char c = mySerial.read();
      //Serial.print(c);  // uncomment to see raw GPS data
      if (gps.encode(c)) 
      {
        newdata = true;
        break;  // uncomment to print new data immediately!
      }
    }
  }
  
  if (newdata) 
  {
    Serial.println("Acquired Data");
    Serial.println("-------------");
    gpsdump(gps);
    Serial.println("-------------");
    Serial.println();
  }
  
}

void gpsdump(TinyGPS &gps)
{
  long lat, lon;
  float  flat, flon;
  unsigned long age, date, time, chars;
  int year;
  byte month, day, hour, minute, second, hundredths;
  unsigned short sentences, failed;



 myFile = SD.open("test.txt", FILE_WRITE);             // opening the SD card

  gps.f_get_position(&flat, &flon);              //getting the latitude and longitude
  Serial.print("Lat/Long(float): "); printFloat(flat, 7);
  Serial.print(", "); printFloat(flon, 7);
  myFile.print(flat,7);myFile.print(",");
  myFile.print(flon,7); myFile.print(","); //printing to SD card

 

  gps.get_datetime(&date, &time, &age);
  Serial.print("Date(ddmmyy): "); Serial.print(date); Serial.print(" Time(hhmmsscc): ");
    Serial.print(time);
 // Serial.print(" Fix age: "); Serial.print(age); Serial.println("ms.");

  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths, &age);
  //Serial.print("Date: "); Serial.print(static_cast<int>(month)); Serial.print("/"); 
   // Serial.print(static_cast<int>(day)); Serial.print("/"); Serial.print(year);
    
  Serial.print("  Time: "); Serial.print(static_cast<int>(hour+5)); Serial.print(":"); // setting time from GMT to IST
  Serial.print(static_cast<int>(minute+30));  Serial.print(":"); Serial.print(static_cast<int>(second));
  Serial.print("."); Serial.print(static_cast<int>(hundredths)); Serial.print(" UTC +05:30 India");
  myFile.print(static_cast<int>(hour+5));  myFile.print(":");    // printing time in SD card
  myFile.print(static_cast<int>(minute+30)); myFile.print(":");
  myFile.print(static_cast<int>(second)); myFile.print(",");
  
    

  Serial.print(gps.course()); Serial.print(" Speed(10^-2 knots): "); Serial.println(gps.speed());
 
  Serial.print(" (kmph): "); printFloat(gps.f_speed_kmph()); Serial.println();
  myFile.print(gps.f_speed_kmph());
  myFile.println();

  gps.stats(&chars, &sentences, &failed);
  Serial.print("Stats: characters: "); Serial.print(chars); Serial.print(" sentences: ");
  Serial.print(sentences); Serial.print(" failed checksum: "); Serial.println(failed);
  myFile.close();
    
}

void printFloat(double number, int digits)
{
  // Handle negative numbers
  if (number < 0.0) 
  {
     Serial.print('-');
     number = -number;
  }

  // Round correctly so that print(1.999, 2) prints as "2.00"
  double rounding = 0.5;
  for (uint8_t i=0; i<digits; ++i)
    rounding /= 10.0;
  
  number += rounding;

  // Extract the integer part of the number and print it
  unsigned long int_part = (unsigned long)number;
  double remainder = number - (double)int_part;
  Serial.print(int_part);

  // Print the decimal point, but only if there are digits beyond
  if (digits > 0)
    Serial.print("."); 

  // Extract digits from the remainder one at a time
  while (digits-- > 0) 
  {
    remainder *= 10.0;
    int toPrint = int(remainder);
    Serial.print(toPrint);
    remainder -= toPrint;
  }


  
}
