#include <Rak3172_Canopus.h>
#include <Arduino.h>
#include "LoRa_SL.h"
#define v1

#define LED_SEND  PA8
#define LED_RECV  PB2
#define LED_SYNC  PA0

long startTime;
bool rx_done = false;
uint16_t sf = 9, bw = 1, cr = 4, preamble = 8, txPower = 22;


void send_cb(void) {
  Serial.println("send callback");
}
  bool init_lora(double freq) {
  if (api.lorawan.nwm.get() != 0) {
    Serial.printf("Set Node device work mode %s\r\n",
                  api.lorawan.nwm.set(0) ? "Success" : "Fail");
    api.system.reboot();
  }

  Serial.println("P2P Start");
  Serial.printf("Hardware ID: %s\r\n", api.system.chipId.get().c_str());
  Serial.printf("Model ID: %s\r\n", api.system.modelId.get().c_str());
  Serial.printf("RUI API Version: %s\r\n",
                api.system.apiVersion.get().c_str());
  Serial.printf("Firmware Version: %s\r\n",
                api.system.firmwareVersion.get().c_str());
  Serial.printf("AT Command Version: %s\r\n",
                api.system.cliVersion.get().c_str());
  Serial.printf("Set P2P mode frequency %3.3f: %s\r\n", (freq / 1e6),
                api.lorawan.pfreq.set(freq) ? "Success" : "Fail");
  Serial.printf("Set P2P mode spreading factor %d: %s\r\n", sf,
                api.lorawan.psf.set(sf) ? "Success" : "Fail");
  Serial.printf("Set P2P mode bandwidth %d: %s\r\n", bw,
                api.lorawan.pbw.set(bw) ? "Success" : "Fail");
  Serial.printf("Set P2P mode code rate 4/%d: %s\r\n", (cr + 5),
                api.lorawan.pcr.set(cr) ? "Success" : "Fail");
  Serial.printf("Set P2P mode preamble length %d: %s\r\n", preamble,
                api.lorawan.ppl.set(preamble) ? "Success" : "Fail");
  Serial.printf("Set P2P mode tx power %d: %s\r\n", txPower,
                api.lorawan.ptp.set(txPower) ? "Success" : "Fail");
  api.lorawan.registerPSendCallback(send_cb);
  delay(1000);
  return true;
}

bool lora_send(int size_msg, uint8_t* msg) {
  Serial.printf("\r\n[Lora]P2P sending...");
  
  bool send_result = api.lorawan.psend(size_msg, msg);

  delay(1000);
  digitalWrite(LED_SYNC, !digitalRead(LED_SYNC));}
