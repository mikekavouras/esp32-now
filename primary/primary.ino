#include <esp_now.h>
#include <WiFi.h>
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#include <RaiseDev.h>

#define ROTARY_ENCODER_A_PIN 32
#define ROTARY_ENCODER_B_PIN 21
#define ROTARY_ENCODER_BUTTON_PIN 22
#define ROTARY_ENCODER_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_STEPS 2

AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

RaiseDev raiseDev;

uint8_t address[] = {0x8C, 0xAA, 0xB5, 0x84, 0xFB, 0xA0};

typedef struct struct_message {
  int value;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

int esp_ok = 1;
int added_peer = 1;
int val = 127;

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  /* Serial.println("sent"); */
}

void IRAM_ATTR readEncoderISR()
{
	rotaryEncoder.readEncoder_ISR();
}

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    added_peer = 0;
    return;
  }

	rotaryEncoder.begin();
	rotaryEncoder.setup(readEncoderISR);
	rotaryEncoder.setBoundaries(0, 255 * ROTARY_ENCODER_STEPS, true);
  rotaryEncoder.disableAcceleration();

  raiseDev.begin();
}

void loop() {
  raiseDev.updateFirmware("mikey", VERSION_STRING_FROM_GIT);

	if (rotaryEncoder.encoderChanged()) {
    val = rotaryEncoder.readEncoder() / ROTARY_ENCODER_STEPS;
    Serial.println(val);
    sendData();
	}
}

void sendData() {
  myData.value = val;
  esp_now_send(address, (uint8_t *) &myData, sizeof(myData));
}
