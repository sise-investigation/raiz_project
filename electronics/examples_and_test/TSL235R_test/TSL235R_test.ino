/**
 * Based on: https://playground.arduino.cc/Main/TSL235R/
 * FireBeetle-ESP32: https://wiki.dfrobot.com/FireBeetle_ESP32_IOT_Microcontroller(V3.0)__Supports_Wi-Fi_&_Bluetooth__SKU__DFR0478#target_4
 * Upload speed: 115200
 */
 
const byte interruptPin = 25;


volatile unsigned long counter = 0;
unsigned long oldCounter = 0;
unsigned long t = 0;
unsigned long last;

portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR handleInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  counter++;
  portEXIT_CRITICAL_ISR(&mux);
}

void setup() {

  Serial.begin(115200);
  Serial.println("Starting TSL235R testing: ");
  pinMode(interruptPin, INPUT_PULLUP);
  digitalWrite(interruptPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, RISING);

}

void loop() {
  if (millis() - last >= 1000)
  {
    last = millis();
    t = counter;
    unsigned long hz = t - oldCounter;
    Serial.print("FREQ: ");
    Serial.print(hz);
    Serial.print("\t = ");
    Serial.print((hz + 50) / 100); // +50 == rounding last digit
    Serial.println(" mW/m2");
    oldCounter = t;
  }
}
