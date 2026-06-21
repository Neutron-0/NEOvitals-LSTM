const int ECG_PIN = A0;

unsigned long lastSample = 0;
const int sampleRate = 250;
const int samplePeriod = 1000 / sampleRate;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long now = millis();

  if (now - lastSample >= samplePeriod) {
    lastSample = now;

    int ecg = analogRead(ECG_PIN);

    Serial.println(ecg);
  }
}