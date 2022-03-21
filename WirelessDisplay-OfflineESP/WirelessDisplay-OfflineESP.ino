#include <RGBmatrixPanel.h> // Required AdaFruit Libraries
#include <Adafruit_GFX.h>

#include "FS.h"
#include "SPI.h" // SD libraries
#include "SD.h"

#define CLK  15   // USE THIS ON ADAFRUIT METRO M0, etc.
#define OE   33
#define LAT 32
#define A   12
#define B   16
#define C   17
#define D   4

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true);

const int chipSelect = 5; // Define cs pin for sd card
char z;
int i, j, x, y, u;
byte r, g, b, t;

File image, imgdata;

void setup() {
  Serial.begin(9600); // Open serial communications and wait for port to open
  while (!Serial) { // wait for serial port to connect
    ; 
  }

  Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!"); // Check to see if SD is recognised
    while (1);
  }
  Serial.println("initialization done.");



  matrix.begin(); // Start the LED display
}

void loop() {
  // put your main code here, to run repeatedly:
  fs::FS SD;
  imgdata = SD.open("imgdata.txt"); // Open image counter file to read how many images are on SD card
  z = imgdata.read();
  t = imgdata.read();
  imgdata.close();
  
  y = z - '0';
  
  if (t >= 'A') { // If value is a hex letter, convert to corresponding number
    t = t - 55;
  }

  else {
    t = t - 48;
  }

  u = t*1000;

  float time1 = micros();
  for (x=1; x<=y; x++) { // Iterate for all images on sd card
    String fname = String(x) + ".txt"; // Form image file name
    image = SD.open(fname); //open image file for reading

     for(i=0; i<32; i++) { // Iterate for 32x32 pixels
      for(j=0; j<32; j++) {
        r = image.read(); // Read values for RGB
        g = image.read();
        b = image.read();
  
        if (r >= 'A') { // If value is a hex letter, convert to corresponding number
          r = r - 55;        
        }

        else {
          r = r - 48;
        }
  
        if (g >= 'A') {
          g = g - 55;  
        }

        else {
          g = g - 48;
        }
  
        if (b >= 'A') {
          b = b - 55;  
        }

        else {
          b = b - 48;
        }
        
        matrix.drawPixel(j, i, matrix.Color444(r, g, b)); // Draw the RGB pixel
      }
    }
    
    matrix.swapBuffers(false);
    image.close();


    //delay(u); // how long each image displays for
  }
  
    float time2 = micros();
    float fps = (7.0/(time2-time1))*1000000.0;

}
