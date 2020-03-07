// ------------------------------------------------------------
// BEP2BLE
// ------------------------------------------------------------
//Define pins
#include <SoftwareSerial.h>
const int rpmPin = 2;       //data[0]
const int speedPin = 3;     //data[1]
const int hibPin = 4;     //data[2]
const int indLeftPin = 5;   //data[7]
const int indRightPin = 6;   //data[8]
const int altPin = 7;     //data[3]
const int tmpPin = 8;     //data[4]
const int oilPin = 9;     //data[5]
const int chkPin = 10;     //data[6]
SoftwareSerial hm10(11, 12);  // RX, TX
const int nPin = 13;      //data[9]
const int gear1Pin = 14;    //data[9]
const int gear2Pin = 15;    //data[9]
const int gear3Pin = 16;    //data[9]
const int gear4Pin = 17;    //data[9]
const int gear5Pin = 18;    //data[9]
const int fuelPin = 19;     //data[10] 
//const int xyz = 20;       //Free pin
//const int xyz = 21;       //Free pin

//Define constants
const int wheelSize = 2000; //millimeter
const int speedSignals = 6; //signals per turn
const int dataSize = 12;
volatile int rpmCount = 0;
volatile int speedCount = 0;
int rpm = 0;
int speed = 0;
int kilometer = 0;
int turnsTotal = 0;
int interval = 1000; //millisec
unsigned long millisLast = 0;
unsigned long millisDiff = 0;
int data[dataSize]; 

void setup(){
    Serial.begin(9600);
    hm10.begin(9600);
    pinMode(rpmPin, INPUT_PULLUP);
    pinMode(speedPin, INPUT_PULLUP);
    pinMode(hibPin, INPUT);
    pinMode(indLeftPin, INPUT);
    pinMode(indRightPin, INPUT);
    pinMode(altPin, INPUT_PULLUP);
    pinMode(tmpPin, INPUT_PULLUP);
    pinMode(oilPin, INPUT_PULLUP);
    pinMode(chkPin, INPUT_PULLUP);
    pinMode(nPin, INPUT_PULLUP);
    pinMode(gear1Pin, INPUT_PULLUP);
    pinMode(gear2Pin, INPUT_PULLUP);
    pinMode(gear3Pin, INPUT_PULLUP);
    pinMode(gear4Pin, INPUT_PULLUP);
    pinMode(gear5Pin, INPUT_PULLUP);
    pinMode(fuelPin, INPUT); 

    digitalWrite(hibPin, LOW);
    digitalWrite(indLeftPin, LOW);
    digitalWrite(indRightPin, LOW);
    digitalWrite(altPin, HIGH);
    digitalWrite(tmpPin, HIGH);
    digitalWrite(oilPin, HIGH);
    digitalWrite(chkPin, HIGH);
    digitalWrite(nPin, HIGH);
    digitalWrite(gear1Pin, HIGH);
    digitalWrite(gear2Pin, HIGH);
    digitalWrite(gear3Pin, HIGH);
    digitalWrite(gear4Pin, HIGH);
    digitalWrite(gear5Pin, HIGH);
  
    attachInterrupt(digitalPinToInterrupt(rpmPin), rpmTrigger, FALLING);
    attachInterrupt(digitalPinToInterrupt(speedPin), speedTrigger, FALLING);
}

void loop(){
  String dataStr = "";

  //Update every one second, this will be equal to reading frecuency (Hz).
    millisDiff = millis() - millisLast;
  if (millisDiff >= interval){ 
      //Set RPM
      rpmCalc();
      //Set Speed
      speedCalc();
   
    // Update last millis
    millisLast = millis(); 
    }
    
    //Set High Beam
    setHib();
    //Set Alternator
    setAlt();
    //Set Water Temperatur
    setTmp();
    //Set Oil preasure
    setOil();
    //Set Choke
    setChk();
    //Set Left Indicator
    setIndLeft();
    //Set Right Indicator
    setIndRight();
    //Set Gear
    setGear();
    //Set Fuel Level
    fuelCalc();
    
    // Build data string
    dataStr = convArray2String(data, dataSize);
    Serial.println(dataStr);
    
    // Convert string to byte
    int len = dataStr.length() + 1;
    byte buf[len];
    dataStr.getBytes(buf, len);
    hm10.write(buf, len);
}

// This code will be executed every time the interrupt 0 (pin2) gets low
void rpmTrigger(){ 
  rpmCount++;
}

// This code will be executed every time the interrupt 1 (pin2) gets low
void speedTrigger(){ 
  speedCount++;
}

String convArray2String(int data[], int sizeOfArray){
  // Convert INT array to STRING
  // Set start flag
    String dataStr = "<";
    for ( int i = 0; i < sizeOfArray; i++ ){
    dataStr = dataStr + String(data[i]) + ";";
    }
  // Set end flag
    dataStr = dataStr + ">";
    return dataStr;
}

void rpmCalc(){
  //Disable interrupt when calculating 
  detachInterrupt(digitalPinToInterrupt(rpmPin));
 
  //Convert frecuency to RPM, note: this works for one interruption per full rotation. For two interrups per full rotation use rpmcount * 30.*/
  rpm = rpmCount * 60;  

  //Store value
  data[0] = rpm;
  
  // Restart the RPM counter
  rpmCount = 0; 

  //enable interrupt
  attachInterrupt(digitalPinToInterrupt(rpmPin), rpmTrigger, FALLING); 
}

void speedCalc(){
  //Disable interrupt when calculating 
  detachInterrupt(digitalPinToInterrupt(speedPin));
  
    // Calulate wheel turns (BMW K sensor provides 6 signals per turn)
    int turns = (speedCount / speedSignals);
    // Calulate total turns
    turnsTotal += turns;
  
    // Calculate speed in km/h
    speed = float(wheelSize)*(float)turns/millisDiff*3.6;
  
    // Calculate kilometer
    kilometer = (float)turnsTotal*(float)wheelSize/1000000.0;

    //Store speed
    data[1] = speed;
  
    //Store kilometer
    data[11] = kilometer;
  
    // Restart the SPEED counter
    speedCount = 0; 

  //enable interrupt
  attachInterrupt(digitalPinToInterrupt(speedPin), speedTrigger, FALLING); 
}

// Set High beam
void setHib(){
  // Get pin values
  int val = digitalRead(hibPin);
  if (val == HIGH){
      data[2] = 1;
  }
  else {
    data[2] = 0;
  }
}

// Set Alternator
void setAlt(){
  // Get pin values
  int val = digitalRead(altPin);
  if (val == LOW){
      data[3] = 1;
   }
   else {
      data[3] = 0; 
   }  
}

// Set Water Temperatur
void setTmp(){
  // Get pin values
  int val = digitalRead(tmpPin);
  if (val == LOW){
      data[4] = 1;
   }
   else {
      data[4] = 0; 
   }    
}

// Set Oil Temperatur
void setOil(){
    // Get pin values
    int val = digitalRead(oilPin);
    if (val == LOW){
      data[5] = 1;
    }
    else {
      data[5] = 0; 
    }  
}

// Set Choke
void setChk(){
    // Get pin values
    int val = digitalRead(chkPin);
    if (val == LOW){
      data[6] = 1;
    }
    else {
      data[6] = 0; 
    }  
}

// Set Indicator Left
void setIndLeft(){
    // Get pin values
    int valIndLeft = digitalRead(indLeftPin);
    if (valIndLeft == HIGH){
      data[7] = 1;
    }
    else {
      data[7] = 0; 
    }    
}

// Set Indicator Right
void setIndRight(){
    // Get pin values
    int valIndRight = digitalRead(indRightPin);
    if (valIndRight == HIGH){
      data[8] = 1;
    }
    else {
      data[8] = 0; 
    }  
}

// Set Gear
void setGear(){
  // Get pin values
  int valGearN = digitalRead(nPin);
  int valGear1 = digitalRead(gear1Pin);
  int valGear2 = digitalRead(gear2Pin);
  int valGear3 = digitalRead(gear3Pin);
  int valGear4 = digitalRead(gear4Pin);
  int valGear5 = digitalRead(gear5Pin);
  
  if (valGearN == LOW){
    data[9] = 0; 
  }
  if (valGear1 == LOW){
    data[9] = 1; 
  }
  if (valGear2 == LOW){
    data[9] = 2; 
  }
  if (valGear3 == LOW){
    data[9] = 3; 
  }
  if (valGear4 == LOW){
    data[9] = 4; 
  }
  if (valGear5 == LOW){
    data[9] = 5; 
  }
}

// Calculate fuel level
void fuelCalc(){
    //Fuel calc
    int raw = 0;
    int Vin = 5;
    float R1 = 100;
    float R2 = 0;
    float buffer = 0;
    float Vr1 = 0;
    float Vr2 = 0;
  
    // Get pin values
    raw = analogRead(fuelPin);
  
    if(raw)
    {
      //Calculate R2 (Fuel level)
      buffer= raw * Vin;
      Vr2 = (buffer)/1024.0;
      Vr1 = Vin - Vr2;
      R2 = (Vr2/Vr1) * R1;
      data[10] = R2;  
    }
}
