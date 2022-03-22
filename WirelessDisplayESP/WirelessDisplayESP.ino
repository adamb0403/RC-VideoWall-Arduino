#include <RGBmatrixPanel.h> // Required AdaFruit Libraries
#include <Adafruit_GFX.h>

#include "FS.h"
#include "SPI.h" // SD libraries
#include "SD.h"
#include <EEPROM.h>
#include "BluetoothSerial.h"

#define CLK  15   // USE THIS ON ADAFRUIT METRO M0, etc.
#define OE   33
#define LAT 32
#define A   12
#define B   16
#define C   17
#define D   4

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true);

BluetoothSerial SerialBT;

//byte IMAGE_COUNT = EEPROM.read(0);
//byte SLIDE_TIME = EEPROM.read(1);

byte IMAGE_COUNT = 10;
byte SLIDE_TIME = 1;

void setup() {
  Serial.begin(115200); // Open serial communications and wait for port to open
  while (!Serial) { // wait for serial port to connect
    ; 
  }

  Serial.print("Initializing SD card...");
  
  const int chipSelect = 5; // Define cs pin for sd card
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!"); // Check to see if SD is recognised
    while (1);
  }
  Serial.println("initialization done.");

  // communication with the BT module on SerialBT
  SerialBT.begin("WIRELESSDISPLAY");

  matrix.begin(); // Start the LED display
}

void loop() {
  // put your main code here, to run repeatedly:
  while(SerialBT.available() < 1) {
    float time1 = micros();
    for (int x=1; x<=IMAGE_COUNT; x++) { // Iterate for all images on sd card
      if (SerialBT.available() > 0) {
        break;
      }
      fs::FS &fs = SD;
      String fname = String(x) + ".txt"; // Form image file name
      File image = fs.open(fname, FILE_READ); //open image file for reading
  
      for(byte rows=0; rows<16; rows++) {
        if (SerialBT.available() > 0) {
          break;
        }
        byte buffers[192];
        byte counter = 0;
        image.read(buffers, sizeof(buffers));
  
        for(byte irow=0; irow<2; irow++) {
          for(byte column=0; column<32; column++) {
            if (SerialBT.available() > 0) {
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
        if (SerialBT.available() > 0) {
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

void readBluetooth(fs::FS &fs) {
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(0, 0);    // start at top left, with one pixel of spacing
  matrix.setTextSize(1);     // size 1 == 8 pixels high
  matrix.setTextWrap(true); 
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.println("Recieving Data...");
  matrix.swapBuffers(false);
  
  int bytesize = 64;
  int chunksize = 3072/bytesize;
  byte btbuffer[bytesize];

  IMAGE_COUNT = SerialBT.read();
  SLIDE_TIME = SerialBT.read();
  EEPROM.write(0, IMAGE_COUNT);
  EEPROM.write(1, SLIDE_TIME);
  EEPROM.commit();
  SerialBT.write(1);

  for(int count=1; count<=IMAGE_COUNT; count++) {
    fs::FS &fs = SD;
    String filename = String(count) + ".txt"; // Form image file name
    File saveBluetooth = fs.open(filename, FILE_WRITE); //open image file for writing
    //delay(50);
    
    for(byte chunks=0; chunks<chunksize; chunks++) {
      for(byte single_byte=0; single_byte<bytesize; single_byte++) {
        while (SerialBT.available() < 1) {
          ;
        }
        btbuffer[single_byte] = SerialBT.read();
      }
      saveBluetooth.write(btbuffer, bytesize);

//      if(chunks+1 % (512/bytesize) == 0) { // When 512 bytes are written to the file, copy the data physically to the SD card using flush()
//        saveBluetooth.flush();
//      }
      SerialBT.write(1);
    }
    saveBluetooth.close();
    Serial.println("Image done");
    
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.setCursor(0, 0);    // start at top left, with one pixel of spacing
    matrix.setTextSize(1);     // size 1 == 8 pixels high
    matrix.setTextWrap(true); 
    matrix.setTextColor(matrix.Color333(7,7,7));
    matrix.println("Recieving Data.");
    int percent = (count*100)/IMAGE_COUNT;
    matrix.println((String) percent + "%");
    matrix.swapBuffers(false);
  }
  serialFlush();
}

void serialFlush(){
  while(SerialBT.available() > 0) {
    char h = SerialBT.read();
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
