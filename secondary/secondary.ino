#include <Adafruit_NeoPixel.h>
#include <esp_now.h>
#include <WiFi.h>
#include <RaiseDev.h>

typedef struct struct_message {
    int value; } struct_message;

struct_message myData;

RaiseDev raiseDev;

#define NUMPIXELS 30
#define LED_PIN 32
Adafruit_NeoPixel strip(NUMPIXELS, LED_PIN, NEO_GRBW + NEO_KHZ800);

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.println(myData.value);
  uint32_t c = wheel(myData.value);
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  strip.begin();

  raiseDev.begin();

  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{
  raiseDev.updateFirmware("mikey", VERSION_STRING_FROM_GIT);
}

uint32_t wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3, 50);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3, 50);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0, 50);
}
