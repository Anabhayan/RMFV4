const int selectPins[] = {10, 11, 12}; 
const int numPins = 3;
const int numStates = 1 << numPins;

void setup() {
  for (int pin : selectPins) {
    pinMode(pin, OUTPUT);
  }
}

void loop() {
  int delayTime = 5.2;
  forwardAndReverseSequence(delayTime, true);
}

void forwardAndReverseSequence(int delayTime, bool direction) {
  int start = direction ? 0 : numStates - 1;
  int end = direction ? numStates : -1;
  int step = direction ? 1 : -1;
  
  for (int i = start; i != end; i += step) {
    for (int j = 0; j < numPins; j++) {
      digitalWrite(selectPins[j], bitRead(i, j));
    }
    delay(delayTime);
  }
}