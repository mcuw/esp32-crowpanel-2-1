/**
 * This example shows how to run with low energy consumption
 *
 * used features:
 * - 0,00000?A power consumption in Deep-Sleep mode
 * - Rotary encoder as a wake-up trigger
 * - press boot + reset button -> waiting for flash update (needed because of deep-sleep) 
 * 
 * Power mode:
 * - CPU                            OFF
 * - Wi-Fi/ BT                      OFF
 * - RTC memory and RTC peripherals ON
 * - ULP co-processor               ON/ OFF
 */
#include <Arduino.h>

RTC_DATA_ATTR int currentStateDT = 0;

// rotary encoder triggered on rotation, just detach interrupt
void IRAM_ATTR encoderISR() {
  detachInterrupt(digitalPinToInterrupt(ENCODER_DT_PIN));
}

// Method to print the reason by which ESP32 has been awaken from sleep
esp_sleep_wakeup_cause_t getWakeupReason(bool print = true){
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  if (print) {
    switch(wakeup_reason)
    {
      case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
      case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
      case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
      case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
      case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
      default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
    }
  }

  return wakeup_reason;
}

void setup()
{
  Serial.begin(115200);

  pinMode(ENCODER_CLK_PIN, INPUT);
  pinMode(ENCODER_DT_PIN, INPUT);

  currentStateDT = digitalRead(ENCODER_DT_PIN);
  
  // Print the wakeup reason as default parameter
  esp_sleep_wakeup_cause_t wakeupReason = getWakeupReason();

  if (wakeupReason != ESP_SLEEP_WAKEUP_EXT0) {
    attachInterrupt(digitalPinToInterrupt(ENCODER_DT_PIN), encoderISR, CHANGE);

    int nextStateDT = !currentStateDT; // set to next state otherwise a reset does not activate deep-sleep in some cases
    esp_sleep_enable_ext0_wakeup(ENCODER_DT_PIN, nextStateDT);

    Serial.println("Going to sleep now");
    Serial.flush();
    esp_deep_sleep_start();
  }
}

void loop()
{
  Serial.println("your code is running...");
  delay(1000);
}
