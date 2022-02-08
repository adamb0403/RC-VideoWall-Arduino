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
int i, j, x, y;
byte r, g, b;

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

  y=4;

  matrix.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  for (x=1; x<=y; x++) {
    String fname = String(x) + ".txt";
    
    myFile = SD.open(fname); //open file for reading

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

    delay(1000);
  }

}
