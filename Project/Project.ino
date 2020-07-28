//Ultrasonic Sensor HC-SR04
//measures distance by emitting 40,000Hz signal and receiving a bounced back signal.
//measures distance in centimeters
//Distance = (full travel time)*(speed of sound) divded by 2
//Switches 2 and 3 are simple on/off toggle switches
//Switch 1 has 3 settings: on, off, and neutral. We chose off to mean off, neutral to mean automatic mode, and on to be on for the headlights.

const int trigPin = 10; //sets trigger pin to pin 10
const int echoPin = 11; //sets echo pin to pin 11
const int sw1 = 2; //Switch 1 is pin 2
const int sw1b = 3; //Switch 1b is pin 3
const int sw2 = 4; //Switch 2 is pin 4
const int sw3 = 5; //Switch 3 is pin 5
const int led1 = 6; //Led 1 (headlight) is pin 6
const int led2 = 7; //Led 2 (fog light) is pin 7
const int led3 = 8; //Led 3 (high beam) is pin 8
const int lt = 80; //Threshold to turn on automatic headlights
const int dk = 100; //Threshold to turn off automatic headlights
int sw1read; //input value for switch 1
int sw1bread; //input value for switch 1b
int sw2read; //input value for switch 2
int sw3read; //input value for switch 3
float a0; //input for PT 1
float a1; //input for PT 2
float a2; //input for PT 3
float a3; //input for PT 4
int arr[50]; //making an array to store distance values
int arrt[50]; //making an array to store times based on the MS clock
int whatTime; //keeps track of the time
int pos = 0; //arbitrary variable that is used to store the position within the array
long duration1; //duration of the pulse for the range sensor 
int distance1; //distance measured by the range sensor 
int changeDistance; //change in distance 
long changeTime; //change in time
float velo; //velocity of  incoming object w/respect to car
int carvelo; //velocity of car
float lastTime;
int lastDist;

void setup() {
    pinMode(trigPin, OUTPUT); //sets trigger pin as Output
    pinMode(echoPin, INPUT); //sets echo pin as Input
    pinMode(sw1, INPUT_PULLUP); //switch 1 is input
    pinMode(sw2, INPUT_PULLUP); //switch 2 is input
    pinMode(sw1b, INPUT_PULLUP); //switch 1b is an input
    pinMode(sw3, INPUT_PULLUP); //switch 3 is input
    pinMode(led1, OUTPUT); //led 1 is output
    pinMode(led2, OUTPUT); //led 2 is output
    pinMode(led3, OUTPUT); //led 3 is output
    Serial.begin(9600);
    digitalWrite(led1, LOW); //setting headlights to low to start
    digitalWrite(led2, LOW); //setting fog lights to low to start
    digitalWrite(led3, LOW); //setting high beams to low to start
}

void loop() {
    sw1read = digitalRead(sw1); //reads switch 1
    sw2read = digitalRead(sw2); //reads switch 2
    sw1bread = digitalRead(sw1b); //reads switch 1b
    sw3read = digitalRead(sw3); //reads switch 3
    //Serial.println(sw1read);
    //Serial.println(sw2read);
    //Serial.println(sw3read);
    if(sw2read == 0){ //if switch 2 is on, turn on fog lights
      digitalWrite(led2, HIGH);
    }
    if(sw2read == 1){ //if switch 2 is off, turn off fog lights
      digitalWrite(led2, LOW);
    }
    if(sw1read == 0){ //if switch 1 is on, turn on headlights
      digitalWrite(led1, HIGH);
    }
    if(sw1read == 1 && sw1bread == 0){ //if switch 1 is off, turn off headlights
      digitalWrite(led1, LOW);
    }
    
    a0 = analogRead(0); //Reads all the analog pins
    Serial.println(a0);
    a1 = analogRead(1);
    Serial.println(a1);
    a2 = analogRead(2);
    Serial.println(a2);
    a3 = analogRead(3);
    Serial.println(a3);
    carvelo = 0; //our car's velo is 0, but could be something else in a different situation

    if(sw1read  == HIGH && sw1bread == HIGH){ //If headlights switch is in automatic, turns on lights if any of 4 phototransistors are reading below the threshold
      if(a0 <= lt){ //if pt1 is below lt, turn lights on
        digitalWrite(led1, HIGH); }
      if(a1 <= lt){ //if pt2 is below lt, turn lights on  
        digitalWrite(led1, HIGH); }
      if(a2 <= lt){ //if pt3 is below lt, turn lights on
        digitalWrite(led1, HIGH); }
      if(a3 <= lt){ //if pt4 is below lt, turn lights on
        digitalWrite(led1, HIGH);}
      if(a0 > dk && a1 > dk && a2 > dk && a3 > dk) //if all of the PTs are above dk, turn  the lights off
        digitalWrite(led1, LOW);
    }
    //the next 7 lines send a pulse from the distance sensor that will bounce back and tell us the distance in cm
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    //sets trigger pin HIGH for 10 microseconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    duration1 = pulseIn(echoPin, HIGH); //reads echopin. returns signal travel time in microseconds
    distance1 = duration1*0.034/2; //distance is half the travel time of the emitted signal times the speed of sound
    //Serial.println(distance1);
    whatTime = millis(); //take current time on ms clock

    if (pos < 19){ //making an array of last 20 values, if it's not full then fill it up first
    arr[pos] = distance1;
    arrt[pos] = whatTime;
    pos++;
    velo = 0;
    }
    if(pos == 19){ //if array is full, add newest value to position 19 and remove oldest value, sliding every value down one position
      for(int i = 0; i < 19; i++){
        arr[i] = arr[i+1];
        arrt[i] = arrt[i+1];
      }
      arr[pos] = distance1;
      arrt[pos] = whatTime;
      changeDistance = arr[0]-arr[pos];
      changeTime = arrt[pos] - arrt[0];
      velo = (float)1000*changeDistance/changeTime; //calculate the velocity of the oncoming object in cm/s
    }
    //Serial.println(distance1); //Serial stuff for tracking the high beam sensor
    //Serial.println(whatTime);
    //Serial.println(arr[0]);
    //Serial.println(changeDistance);
    //Serial.println(changeTime);
    //Serial.println(velo);
    Serial.println("________________________________________");
    


    if(sw3read == LOW){ //check if switch is on for high beams
      if(velo > 10 + carvelo) //dim the high beams if oncoming velocity is faster than car velocity (with a buffer of 2 to avoid flickering)
        digitalWrite(led3, LOW);
      else if(velo<=carvelo)
        digitalWrite(led3, HIGH);//otherwise, turn high beams on
    }else{
      digitalWrite(led3, LOW); //if switch is off, high beams always off
    }
}
