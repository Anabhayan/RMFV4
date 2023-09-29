#define ZeroError 0.8
int j;


//-------Decoder logic
const int decoderSelectPins[] = { 10, 11, 12 };
const int numPins = 3;
const int numStates = 1 << numPins - 2; //only 6 leds 


//Frequency
const int RphaseIntPin = 2;
float frequency = 0.0;
volatile unsigned long startTime = 0;
volatile unsigned long endTime = 0;
volatile bool risingEdge = false;
volatile bool measureFrequency = false;
const float minFrequency = 0.0;
const float maxFrequency = 15.0;

void setup() {
  pinMode(RphaseIntPin, INPUT_PULLUP);

  for (int pin : decoderSelectPins) {
    pinMode(pin, OUTPUT);
  }
  attachInterrupt(digitalPinToInterrupt(RphaseIntPin), handleInterrupt, CHANGE);
  Serial.begin(9600);
}

void loop() {
  measureFrequency = true;
  delay(500);
  measureFrequency = false;

  if (isValidFrequency(frequency)) {
    Serial.print("Frequency: ");
    Serial.println(frequency);


    float timePeriod = 1 / frequency;

    delay(timePeriod / 4);  // Flux - voltage delay.
    delay(timePeriod / 4);  // wait for R peak.


    for (j = 0; j < numPins; j++) {
      digitalWrite(decoderSelectPins[j], bitRead(0, j));
    }  // led 1 is fired
    delay(timePeriod / 8);  // led 1 burns for timePeriod/8 ms (8ms randomly chosen)
    for (j = 0; j < numPins; j++) {
      digitalWrite(decoderSelectPins[j], bitRead(7, j));
    }   // led 1 is turned off
    forwardAndReverseSequence(timePeriod , 1);
  }

}

bool isValidFrequency(float freq) {
  return freq >= minFrequency && freq <= maxFrequency;
}

void handleInterrupt() {
  delayMicroseconds(100);  // Give some time for the processor to handle interrupts.
  if (measureFrequency) {
    int sensorValue = digitalRead(RphaseIntPin);

    if (sensorValue == HIGH && !risingEdge) {
      startTime = micros();
      risingEdge = true;
    } else if (sensorValue == LOW && risingEdge) {
      endTime = micros();
      risingEdge = false;
      frequency = 1.0 / ((endTime - startTime) * 1e-6);
      frequency = frequency / 2 + ZeroError;
    }
  }
}

void forwardAndReverseSequence(int delayTime, bool direction) {
  int start = direction ? 0 : numStates - 1;
  int end = direction ? numStates : -1;
  int step = direction ? 1 : -1; 

  for (int i = start; i != end; i += step) {

    delay(delayTime / 24);

    for (j = 0; j < numPins; j++) {
      digitalWrite(decoderSelectPins[j], bitRead(i, j));
    }  // led 'i' is fired

    delay(delayTime / 8);  // led 'i' burns for timePeriod/8 ms 

    for (j = 0; j < numPins; j++) {
      digitalWrite(decoderSelectPins[j], bitRead(7, j));
    }  // led 'i' is turned off

  }
}
