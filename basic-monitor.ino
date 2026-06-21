const int loPlusPin = 10;
const int loMinusPin = 11;
const int ecgAnalogPin = A0;
const int buzzerPin = 3;

// Signal tracking variables
int maxSignal = 0;
int minSignal = 1023;
int adaptiveThreshold = 600;
bool isPeakHandled = false;

// Time tracking
unsigned long lastBeatTime = 0;
unsigned long lastBuzzerTime = 0;
unsigned long lastWindowReset = 0;

// BPM Calculation
int accurateBPM = 72; 
const int numReads = 6;
int bpmArray[numReads] = {72, 72, 72, 72, 72, 72};
int bpmIndex = 0;

void setup() {
  Serial.begin(9600);
  pinMode(loPlusPin, INPUT);
  pinMode(loMinusPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  unsigned long currentTime = millis();

  // 1. Constant 1-Second Metronome Beep (Independent)
  if (currentTime - lastBuzzerTime >= 1000) {
    digitalWrite(buzzerPin, HIGH);
    delay(40); 
    digitalWrite(buzzerPin, LOW);
    lastBuzzerTime = currentTime; 
  }

  // 2. Lead-Off Check
  if ((digitalRead(loPlusPin) == 1) || (digitalRead(loMinusPin) == 1)) {
    return; 
  }

  int ecgValue = analogRead(ecgAnalogPin);

  // 3. Track Max/Min points dynamically to map the true shape of your wave
  if (ecgValue > maxSignal) maxSignal = ecgValue;
  if (ecgValue < minSignal) minSignal = ecgValue;

  // Readjust adaptive limits every 2 seconds to handle shifting baselines
  if (currentTime - lastWindowReset > 2000) {
    int pkToPk = maxSignal - minSignal;
    if (pkToPk > 40) {
      adaptiveThreshold = minSignal + (pkToPk * 0.70); // Set trigger line at 70% height of your actual wave
    }
    maxSignal = ecgValue;
    minSignal = ecgValue;
    lastWindowReset = currentTime;
  }

  // 4. Solid Trigger Logic with a 500ms Lockout Window
  if (ecgValue > adaptiveThreshold) {
    // Only register a beat if at least 500ms has passed since the last one
    if (!isPeakHandled && (currentTime - lastBeatTime > 500)) {
      unsigned long timeBetweenBeats = currentTime - lastBeatTime;
      lastBeatTime = currentTime; 
      
      int rawBPM = 60000 / timeBetweenBeats;

      // Restrict calculations to physically possible resting rates (45 to 130 BPM)
      if (rawBPM >= 45 && rawBPM <= 130) {
        bpmArray[bpmIndex] = rawBPM;
        bpmIndex = (bpmIndex + 1) % numReads;

        // Calculate a strict average
        long sum = 0;
        for (int i = 0; i < numReads; i++) {
          sum += bpmArray[i];
        }
        accurateBPM = sum / numReads;
      }
      isPeakHandled = true;
    }
  } else if (ecgValue < (adaptiveThreshold - 25)) {
    isPeakHandled = false; // Reset trigger state only when signal safely drops
  }

  // 5. Output Data
  if (currentTime % 40 == 0) {
    Serial.print("Raw_ECG:");
    Serial.print(ecgValue);
    Serial.print("\tTrigger_Line:");
    Serial.print(adaptiveThreshold);
    Serial.print("\tAccurate_BPM:");
    Serial.println(accurateBPM);
  }

  delay(2);
}