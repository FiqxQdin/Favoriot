const int sensor_pin = 27;     // Light sensor pin = GP27
const float max_value = 1023;  // 10-bit ADC
const float max_volt = 3.3;    // Max voltage

void setup() {
  Serial.begin(115200);

  pinMode(sensor_pin, INPUT);
}

void loop() {

  int raw_value = analogRead(sensor_pin); 
  float volt_value = (raw_value / max_value) * max_volt; 

  Serial.println(raw_value);
  Serial.println(volt_value);
  Serial.println("");

  delay(1000);
}