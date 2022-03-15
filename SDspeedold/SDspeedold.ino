#include <SD.h>

File file;

void setup() {
  Serial.begin(9600);
  SD.begin(53);
  file = SD.open("WR_TEST1.TXT", FILE_WRITE);

  Serial.print("start");
  while(millis() < 1000);  // delay so mills() is four digits

  for (uint8_t i = 0; i < 100; i++) {
    file.println(millis());
  }
  file.close();
  Serial.print(millis());
}

void loop() {}
