#include <RGBmatrixPanel.h> // Include required AdaFruit Libraries
#include <Adafruit_GFX.h>

#include <SPI.h> // SD libraries
#include <SD.h>
#include <EEPROM.h> // EEPROM libraries

#define CLK 11 // Define pins for display
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, true); // Initialise matrix with defined pins and use double buffering

byte IMAGE_COUNT = EEPROM.read(0); // Read number of images from EEPROM
byte SLIDE_TIME = EEPROM.read(1); // Read slide time from EEPROM

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

  Serial1.begin(115200); // communication with the BT module on serial1

  matrix.begin(); // Start the LED display
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial1.available() < 1) { // Repeat loop while there is nothing in the Serial1 buffer
    float time1 = micros();
    for (int x=1; x<=IMAGE_COUNT; x++) { // Iterate for all images on sd card
      if (Serial1.available() > 0) { // Check for bluetooth data received in Serial1 buffer
        break;
      }
      String fname = String(x) + ".txt"; // Form image file name
      File image = SD.open(fname); // Open image file for reading
  
      for(byte rows=0; rows<16; rows++) { // Iterate for 16x2 rows (Using 192 buffer)
        byte buffers[192];
        byte counter = 0;
        image.read(buffers, sizeof(buffers)); // Read 192 bytes (64 pixels - 2 rows) from SD card into a buffer
  
        for(byte irow=0; irow<2; irow++) { // Iterate for all 64 pixels in buffer (2 rows = 32x2 = 64 pixels)
          for(byte column=0; column<32; column++) {
            // Call hexCheck function for each colour channel to convert ascii characters to int equivalents
            // drawPixel in 4bit colour to output to display buffer
            matrix.drawPixel(irow+(rows*2), column, matrix.Color444(hexCheck(buffers[counter]), hexCheck(buffers[counter+1]), hexCheck(buffers[counter+2])));
            counter+=3; // Increment counter to next set of bytes for next pixel
          }
        }
      }
    
      matrix.swapBuffers(false); // Output the buffer to the display LEDs
      image.close();

      delay(SLIDE_TIME*1000); // how long each image displays for
      }
    }
    float time2 = micros();
    float fps = (IMAGE_COUNT/(time2-time1))*1000000.0;
    Serial.println(fps); // Print the FPS to the Serial Monitor
  }

  readBluetooth(); // If Serail1 buffer has data, call function to read BT data
}

void readBluetooth() {
  // Output "Receiving data" message to display
  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.setCursor(0, 0);
  matrix.setTextSize(1);
  matrix.setTextWrap(true); 
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.println("Recieving Data...");
  matrix.swapBuffers(false);
  
  int bytesize = 64; // Define how many bytes will be incoming per chunk
  int chunksize = 3072/bytesize; // Define how many chunks there will be per image
  byte btbuffer[bytesize]; // Initialise a local buffer the size of each chunk

  // Read image count and slide time from BT and store in EEPROM
  IMAGE_COUNT = Serial1.read();
  SLIDE_TIME = Serial1.read();
  EEPROM.write(0, IMAGE_COUNT);
  EEPROM.write(1, SLIDE_TIME);
  Serial1.write(1); // Send byte to App to confirm data has been processed

  for(int count=1; count<=IMAGE_COUNT; count++) { // Iterate for all incoming images
    String filename = String(count) + ".txt"; // Form image file name
    
    File saveBluetooth = SD.open(filename, O_CREAT | O_WRITE | O_TRUNC); // Open image file for writing
    
    for(byte chunks=0; chunks<chunksize; chunks++) { // Iterate for all chunks
      int x1 = micros();
      for(byte single_byte=0; single_byte<bytesize; single_byte++) { // Iterate for all bytes per chunk
        
        while (Serial1.available() < 1) { // Hold while there is no data in the Serial1 buffer
          ;
        }
        
        btbuffer[single_byte] = Serial1.read(); // Store byte from bluetooth into the local buffer
      }
      int x2 = micros();
      saveBluetooth.write(btbuffer, bytesize); // Write the local buffer to the file

      if(chunks+1 % (512/bytesize) == 0) { // When 512 bytes have been written to the file, copy the data physically to the SD card using flush()
        saveBluetooth.flush();
      }
      Serial1.write(1); // Send byte to App to confirm a chunk has been processed
    }
    saveBluetooth.close();
    Serial.println("Image done");

    // Output a progress percentage counter to matrix based on how many images have been processed
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.setCursor(0, 0);
    matrix.setTextSize(1);
    matrix.setTextWrap(true); 
    matrix.setTextColor(matrix.Color333(7,7,7));
    matrix.println("Recieving Data.");
    int percent = (count*100)/IMAGE_COUNT;
    matrix.println((String) percent + "%");
    matrix.swapBuffers(false);
  }
  serialFlush(); // Call function to clear the Serial1 buffer
}

void serialFlush(){
  while(Serial1.available() > 0) {
    char h = Serial1.read(); // Read the buffer while it is larger than 0
  }
}

byte hexCheck(byte x) {
  if (x >= 'A') { // If value is a hex letter, convert to corresponding number
      x = x - 55;
  }
  else { 
      x = x - 48; // If value is not a hex letter, convert to corresponding number
  }
  return(x);
}
