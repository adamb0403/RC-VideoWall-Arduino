#include <RGBmatrixPanel.h> // Required AdaFruit Libraries
#include <Adafruit_GFX.h>
//#include "SdFat.h"

#include <SPI.h> // SD libraries
#include <SD.h>

#define CLK 11 // Define pins for display
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true);

const int chipSelect = 53; // Define cs pin for sd card
char z;
int i, j, x, y, u;
byte r, g, b, t, SLIDE_TIME=0, IMAGE_COUNT=7;

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

  // communication with the BT module on serial1
  Serial1.begin(38400);

  matrix.begin(); // Start the LED display

  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial1.available() > 0) {
    readBluetooth();
  }
  
//  imgdata = SD.open("imgdata.txt"); // Open image counter file to read how many images are on SD card
//  z = imgdata.read();
//  t = imgdata.read();
//  imgdata.close();
//  
//  y = z - '0';
//  
//  if (t >= 'A') { // If value is a hex letter, convert to corresponding number
//    t = t - 55;        
//  }
//
//  else {
//    t = t - 48;
//  }
//
//  u = t*1000;
  
  for (x=1; x<=IMAGE_COUNT; x++) { // Iterate for all images on sd card
    String fname = String(x) + ".txt"; // Form image file name
    image = SD.open(fname); //open image file for reading

    for(byte rows=0; rows<32; rows++) {
      byte buffers[96];
      byte counter = 0;
      image.read(buffers, sizeof(buffers));
      
      for(byte column=0; column<32; column++) {
        matrix.drawPixel(column, rows, matrix.Color444(hexCheck(buffers[counter]), hexCheck(buffers[counter+1]), hexCheck(buffers[counter+2]))); // Draw the RGB pixel
        counter+=3;
      }
    }

//     for(i=0; i<32; i++) { // Iterate for 32x32 pixels
//      for(j=0; j<32; j++) {
//        r = image.read(); // Read values for RGB
//        g = image.read();
//        b = image.read();
//  
//        if (r >= 'A') { // If value is a hex letter, convert to corresponding number
//          r = r - 55;        
//        }
//
//        else {
//          r = r - 48;
//        }
//  
//        if (g >= 'A') {
//          g = g - 55;  
//        }
//
//        else {
//          g = g - 48;
//        }
//  
//        if (b >= 'A') {
//          b = b - 55;  
//        }
//
//        else {
//          b = b - 48;
//        }
//        
//        matrix.drawPixel(j, i, matrix.Color444(r, g, b)); // Draw the RGB pixel
//      }
//    }
    
    matrix.swapBuffers(false);
    image.close();

    //delay(SLIDE_TIME*1000); // how long each image displays for
  }
}

void readBluetooth() {
  IMAGE_COUNT = Serial1.read();
  SLIDE_TIME = Serial1.read();

  Serial.print(IMAGE_COUNT);
  Serial.print(SLIDE_TIME);

  for(byte count=1; count<=IMAGE_COUNT; count++) {
    String filename = String(count) + ".txt"; // Form image file name
    
    File saveBluetooth = SD.open(filename, O_CREAT | O_TRUNC | O_WRITE); //open image file for writing

    for(byte chunks=0; chunks<48; chunks++) {
      for(byte single_byte=0; single_byte<64; single_byte++) {
        Serial.write( Serial1.read() );
        //saveBluetooth.write(Serial1.read());
      }
      delay(500);
    }

    saveBluetooth.close();
  }
  serialFlush();
}

void serialFlush(){
  while(Serial1.available() > 0) {
    char h = Serial1.read();
  }
}

//int File::read(void *buf, uint16_t nbyte) {
//  if (_file) 
//    return _file->read(buf, nbyte);
//  return 0;
//}

byte hexCheck(byte x) {
  if (x >= 'A') { // If value is a hex letter, convert to corresponding number
      x = x - 55;
  }
  else {
      x = x - 48;
  }
  return(x);
}
