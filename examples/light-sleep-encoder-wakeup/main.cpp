/**
 * This example shows how to run with low energy consumption
 *
 * used features:
 * - 0,00000?A power consumption in Light-Sleep mode
 * - Rotary encoder as a wake-up trigger
 * - you can activate the sleep-mode for example with the boot button
 * 
 * Power mode:
 * - CPU                            PAUSE, continues after wake-up
 * - Wi-Fi/ BT                      OFF
 * - RTC memory and RTC peripherals ON
 * - ULP co-processor               ON
 */
#include <Arduino.h>

int currentStateDT = 0;
int loopCounter = 0;

volatile bool sleepRequested = false;

void IRAM_ATTR bootButtonISR() {
  sleepRequested = true;
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
      default : Serial.printf("Wakeup was not caused by light sleep: %d\n",wakeup_reason); break;
    }
  }

  return wakeup_reason;
}

void setup()
{
  Serial.begin(115200);

  pinMode(ENCODER_CLK_PIN, INPUT);
  pinMode(ENCODER_DT_PIN, INPUT);

  // setup boot button as a sleep activator
  pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BOOT_BUTTON_PIN), bootButtonISR, FALLING);
}

void loop()
{
  currentStateDT = digitalRead(ENCODER_DT_PIN);
  ++loopCounter;

  // ALWAYS reconfigure, with the current read state
  int nextStateDT = !currentStateDT;
  esp_err_t result = esp_sleep_enable_ext0_wakeup(ENCODER_DT_PIN, nextStateDT);
  // uncomment to handle on error
  // if (result == ESP_OK) {
  //     Serial.println("Rotary Encoder Wake-Up set successfully as wake-up source.");
  // } else {
  //     Serial.println("Failed to set Rotary Encoder Wake-Up as wake-up source.");
  // }

  if (sleepRequested) {
    sleepRequested = false;
    delay(30); // Debounce after interrupt, before sleep
    Serial.println("Going to sleep now");
    Serial.flush();
    esp_light_sleep_start();
    getWakeupReason();
  }

  // REPLACE here with your app...
  Serial.print("Your code is running. loop count: ");
  Serial.println(loopCounter);
  delay(1000);
  // REPLACE here with your app.
}
