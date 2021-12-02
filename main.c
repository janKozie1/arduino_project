#define maxM 500
#define threshold 320

#define minMToCheck 100
#define minValleyLength 80 

#define ledOffset 11
#define ledAmount 2

const int sensorPin = A0;   
const int readyLedPin = 13;  
const byte btnPin = 2;

volatile int sensorValue = 0;
volatile byte shouldDetect = LOW;
volatile byte lightIsOn = LOW;
volatile byte prevLightIsOn = 0;

int m[maxM] = {0};
int index = 0;
int ledPin = 11;   


byte isReady(int index) {
 return index == maxM - 1;
}

void cycleLed() {
  digitalWrite(ledPin, LOW);
  ledPin = (((ledPin - ledOffset) + 1) % ledAmount) + ledOffset;
}

void writeAndPushBack(int value, int index) {
  if (index == maxM - 1) {
    for (int i = 0; i < index; i++) {
      m[i] = m[i+1];
    }
  }
  
  m[index] = value;
}

boolean isPeak(int index) {
  unsigned long int total = 0;
  int avg = 0;

  int value = m[index];

  for (int i = 0; i < index; i++) {
    total += m[i];
  }

  avg = total / index;  

  if (value <= avg) {
    return false;  
  }

  return (value - avg) > threshold;
}

boolean isNewPeak(int index) {
  if (!isPeak(index)) {
    return false;  
  }
  
  for (int i = index; i > index - minMToCheck; i--) {
   boolean wasValley = true;
   
   for (int j = i; j > i - minValleyLength; j--){
     if (isPeak(j)) {
       wasValley = false;
       break; 
     }
   }

   if (wasValley) {
    return true; 
   }
  }

  return false;
}

void setup() {
  // declare the ledPin as an OUTPUT:
  // Serial.begin(9600);

  for (int i = 0; i < ledAmount; i++) {
      pinMode(ledOffset + i, OUTPUT);
  }
 
  pinMode(readyLedPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(btnPin), cycleLed, RISING);
}

void loop() {
  byte ready = isReady(index);
  digitalWrite(readyLedPin, !ready);

  writeAndPushBack(analogRead(sensorPin), index);
  prevLightIsOn = lightIsOn;

  if (ready) {
     if (isNewPeak(index)) {
        lightIsOn = !lightIsOn;  
     }
  
     digitalWrite(ledPin, lightIsOn);
  } else {
     index++;  
  }

  if (prevLightIsOn != lightIsOn) {
    delay(200);  
  }
}