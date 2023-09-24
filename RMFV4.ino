const int interruptPin = 3;
volatile unsigned long startTime = 0;
volatile unsigned long endTime = 0;
volatile bool risingEdge = false;
volatile bool measureFrequency = false;

const float minFrequency = 0.5;
const float maxFrequency = 60; 
const int sampleSize = 3;
float samples[sampleSize];
int sampleIndex = 0;
float lastValidFrequency = -1;

void setup() {
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, CHANGE);
  Serial.begin(9600);
}

void loop() {
  if (millis() % 2000 == 0) { // Check frequency changes every 3 seconds
    measureFrequency = true; // Enable interrupt
    delay(1800);
    measureFrequency = false; // Disable interrupt
    float frequency = getMostOccurring(samples, sampleSize); // Get the most occurring frequency from samples
    if (frequency >= minFrequency && frequency <= maxFrequency) { // Check if frequency is within range
      Serial.print("Frequency: ");
      Serial.println(frequency);
      lastValidFrequency = frequency; // Update the last valid frequency
    } else {
      Serial.println("Invalid frequency");
      if (lastValidFrequency != -1) { // If there is a last valid frequency
        Serial.print("Defaulting to last valid frequency: ");
        Serial.println(lastValidFrequency);
      }
    }
    sampleIndex = 0; // Reset sample index
  }
}

void handleInterrupt() {
  if (measureFrequency) {
    int sensorValue = digitalRead(interruptPin);

    if (sensorValue == HIGH && !risingEdge) {
      startTime = micros();
      risingEdge = true;
    } else if (sensorValue == LOW && risingEdge) {
      endTime = micros();
      risingEdge = false;
      float frequency = 1.0 / ((endTime - startTime) * 1e-6);
      samples[sampleIndex] = frequency/2; // Store the frequency in samples array
      sampleIndex++; // Increment sample index
    }
  }
}

float getMostOccurring(float arr[], int n) {
  float maxCount = 0; // Maximum count of an element
  float result = -1; // Resultant element
  for (int i = 0; i < n; i++) {
    int count = 1; // Count of current element
    for (int j = i + 1; j < n; j++) {
      if (arr[i] == arr[j]) { // If current element matches with another element
        count++; // Increment count
      }
    }
    if (count > maxCount) { // If count is greater than maxCount
      maxCount = count; // Update maxCount
      result = arr[i]; // Update result
    }
  }
  return result;
}
