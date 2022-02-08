#include <RGBmatrixPanel.h>
#include <Adafruit_GFX.h>

#include <SPI.h>
#include <SD.h>

#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

const int chipSelect = 53;

File myFile;

void setup() {
  // put your setup code here, to run once:
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  

  //open file for reading
  myFile = SD.open("3.txt");

  matrix.begin();

  int i, j;
  byte r, g, b;

 /* while (myFile.available()) {
     Serial.write(getVal(myFile.read()));
    }*/
  
  for(i=0; i<32; i++) {
    for(j=0; j<32; j++) {
      r = myFile.read();
      g = myFile.read();
      b = myFile.read();

      if (r >= 'A') {
        r = r - 55;        
      }

      if (g >= 'A') {
        g = g - 55;  
      }

      if (b >= 'A') {
        b = b - 55;  
      }
      
      matrix.drawPixel(j, i, matrix.Color444(r, g, b));
    }
  }

  myFile.close();
}

void loop() {
  // put your main code here, to run repeatedly:

}

/*byte getVal(char c)
{
   if(c >= '0' && c <= '9')
     return (byte)(c);
   else
     return (byte)(c-'a'+10);
}*/
/*
int changecol(char x) {
  //x=x+17;
  //return (char)(x);
  
  int y;
  if(x='A') {
    y=10;
    return (int) y;
  }
  
  else if(x='B') {
    y=11;
    return (int) y;
  }

  else if(x='C') {
    y=12;
    return (int) y;
  }

  else if(x='D') {
    y=13;
    return (int) y;
  }

  else if(x='E') {
    y=14;
    return (int) y;
  }

  else if(x='F') {
    y=15;
    return (int) y;
  }

  else {
    y=5;
    return (int) y;
  }

  
}*/
