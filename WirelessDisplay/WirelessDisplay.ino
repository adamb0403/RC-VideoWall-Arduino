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

byte SLIDE_TIME = 2, IMAGE_COUNT = 8;

void setup() {
  Serial.begin(9600); // Open serial communications and wait for port to open
  while (!Serial) { // wait for serial port to connect
    ; 
  }

  Serial.print("Initializing SD card...");
  
  const int chipSelect = 53; // Define cs pin for sd card
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!"); // Check to see if SD is recognised
    while (1);
  }
  Serial.println("initialization done.");

  // communication with the BT module on serial1
  Serial1.begin(115200);

  matrix.begin(); // Start the LED display
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial1.available() < 1) {
    float time1 = micros();
    for (int x=1; x<=IMAGE_COUNT; x++) { // Iterate for all images on sd card
      if (Serial1.available() > 0) {
        break;
      }
      String fname = String(x) + ".txt"; // Form image file name
      File image = SD.open(fname); //open image file for reading
  
      for(byte rows=0; rows<16; rows++) {
        if (Serial1.available() > 0) {
          break;
        }
        byte buffers[192];
        byte counter = 0;
        image.read(buffers, sizeof(buffers));
  
        for(byte irow=0; irow<2; irow++) {
          for(byte column=0; column<32; column++) {
            if (Serial1.available() > 0) {
              break;
            }
            matrix.drawPixel(irow+(rows*2), column, matrix.Color444(hexCheck(buffers[counter]), hexCheck(buffers[counter+1]), hexCheck(buffers[counter+2]))); // Draw the RGB pixel
            counter+=3;
          }
        }
      }
    
      matrix.swapBuffers(false);
      image.close();

      for(int d=0; d<1000; d++) {
        if (Serial1.available() > 0) {
          break;
        }
        delay(SLIDE_TIME); // how long each image displays for
      }
    }
    float time2 = micros();
    float fps = (IMAGE_COUNT/(time2-time1))*1000000.0;
    //Serial.println(fps);
  }

  readBluetooth();
}

void readBluetooth() {
  int bytesize = 64;
  int chunksize = 3072/bytesize;
  byte btbuffer[bytesize];

  IMAGE_COUNT = Serial1.read();
  SLIDE_TIME = Serial1.read();

  while (Serial1.available() < 1) {
    ;
  }

  for(int count=1; count<=IMAGE_COUNT; count++) {
    String filename = String(count) + ".txt"; // Form image file name

//    if (SD.exists(filename)) {
//      SD.remove(filename);
//    }
    
    File saveBluetooth = SD.open(filename, O_CREAT | O_WRITE | O_TRUNC); //open image file for writing
    
    for(byte chunks=0; chunks<chunksize; chunks++) {
      int x1 = micros();
      for(byte single_byte=0; single_byte<bytesize; single_byte++) {
        //Serial.write( Serial1.read() );
        btbuffer[single_byte] = Serial1.read();
      }
      int x2 = micros();
      //Serial.println(x2-x1);
      saveBluetooth.write(btbuffer, bytesize);
      //Serial.println(chunks);

      if(chunks+1 % (512/bytesize) == 0) { // When 512 bytes are written to the file, copy the data physically to the SD card using flush()
        saveBluetooth.flush();
      }
      
      delay(100);
    }
    saveBluetooth.close();
    Serial.println("Upload done");
  }
  serialFlush();
}

void serialFlush(){
  while(Serial1.available() > 0) {
    char h = Serial1.read();
  }
}

byte hexCheck(byte x) {
  if (x >= 'A') { // If value is a hex letter, convert to corresponding number
      x = x - 55;
  }
  else {
      x = x - 48;
  }
  return(x);
}
