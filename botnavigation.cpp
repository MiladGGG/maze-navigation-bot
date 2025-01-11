#include <Servo.h>




/*
Servo Speeds:
1300 - full speed clockwise
1400 - half speed clockwise
1500 - stationary
1600 - half speed anti-clockwise
1700 - full speed anti-clockwise
*/




/*
Robot dimensions:
Length: 15cm
Width: 11cm
Wheels: 6cm from front
*/




/*
Maze dimensions:
Width: 20cm
*/




/*
Turn times (at 300 speed):
Left turn: 540ms
Right turn: 523ms
180 turn:
720 turn:
*/




Servo servoLeft;
Servo servoRight;
int ir_sensor_state = -1; // reading of IR sensor




// const int left_turn_time = 370; 370 * 8 = 2960 therefore 2960 for 720 turn
// const int right_turn_time = 360;




const int left_turn_time = 540;
const int right_turn_time = 523;




// Centre
const int C_RED_LED_PIN = A1; // LED
const int C_IR_RX_PIN = 7; // receiver pin
const int C_IR_TX_PIN = 6; // transmitter pin
// Left
const int L_RED_LED_PIN = A2;
const int L_IR_RX_PIN = 11;
const int L_IR_TX_PIN = 10;
// Right
const int R_RED_LED_PIN = A0;
const int R_IR_RX_PIN = 3;
const int R_IR_TX_PIN = 2;




const int L_SERVO_STALL_PWM = 1500;
const int R_SERVO_STALL_PWM = 1500;




const int C_IR_ZONE_DISTANCES[] = {40, 54, 63, 79, 91, 400};
const int L_IR_ZONE_DISTANCES[] = {50, 61, 75, 90, 105, 400};
const int R_IR_ZONE_DISTANCES[] = {54, 61, 68, 83, 98, 400};




const int MAX_SPEED = 300;




bool isTurning;
bool leftWallDetected;
bool rightWallDetected;
bool frontWallDetected;

bool finishedFlag;
bool flash = false;
int elapsedTime = 0;


// TODO
// allow weird rotation on start up (180 2 point mid strategy)
// start turning early
// As soon as left wall open, start turning at slow speed then speed up




void setup()
{
    Serial.begin(9600);




    // Initialize centre LED and IR sensors
    pinMode(C_RED_LED_PIN, OUTPUT);
    pinMode(C_IR_RX_PIN, INPUT);
    pinMode(C_IR_TX_PIN, OUTPUT);




    // Initialize left LED and IR sensors
    pinMode(L_RED_LED_PIN, OUTPUT);
    pinMode(L_IR_RX_PIN, INPUT);
    pinMode(L_IR_TX_PIN, OUTPUT);




    // Initialize right LED and IR sensors
    pinMode(R_RED_LED_PIN, OUTPUT);
    pinMode(R_IR_RX_PIN, INPUT);
    pinMode(R_IR_TX_PIN, OUTPUT);




    // Attach servos
    servoLeft.attach(13);
    servoRight.attach(12);




    // set isTurning value to false
    isTurning = false;
    leftWallDetected = false;
    rightWallDetected = false;
}








void loop()
{
    int centreIRZone = getWallZone(C_IR_RX_PIN, C_IR_TX_PIN);
    int leftIRZone = getWallZone(L_IR_RX_PIN, L_IR_TX_PIN);
    int rightIRZone = getWallZone(R_IR_RX_PIN, R_IR_TX_PIN);
    int centerDistance = C_IR_ZONE_DISTANCES[centreIRZone];
    int leftDistance = L_IR_ZONE_DISTANCES[leftIRZone];
    int rightDistance = R_IR_ZONE_DISTANCES[rightIRZone];
    // Serial.println("Front zone: " + String(centreIRZone));
    // Serial.println("Left zone: " + String(leftIRZone));
    // Serial.println("Right zone: " + String(rightIRZone));
    // Serial.println();




    // toggle xWallDetected booleans based front, left and right sensors




    // front
    if (centreIRZone < 5) {
        frontWallDetected = true;
    } else {
        frontWallDetected = false;
    }
    // left
    if (leftIRZone < 5) {
        leftWallDetected = true;
    } else {
        leftWallDetected = false;
    }
    // right
    if (rightIRZone < 5) {
        rightWallDetected = true;
    } else {
        rightWallDetected = false;
    }




    // Activate LEDs based on walls detected
    digitalWrite(C_RED_LED_PIN, frontWallDetected);
    digitalWrite(L_RED_LED_PIN, leftWallDetected);
    digitalWrite(R_RED_LED_PIN, rightWallDetected);


    /*
    #########################################
    TURNING LOGIC
    #########################################
    */




    // ⛔️ = wall
    // ✅ = no wall

    if(finishedFlag == false){
        Serial.println(isTurning);
        // Left wall:⛔️ Right wall:✅
        // Therefore GO RIGHT


        // isTurrning
        if(leftWallDetected && !rightWallDetected && isTurning == false) { // opening on the right
            delay(190); // wait a bit before turning
            Serial.println("RIGHT");
            goForwardRight();
            isTurning = true;
        }
        // Left wall:✅  Right wall:⛔️
        // Therefore GO LEFT
        else if(!leftWallDetected && rightWallDetected && isTurning == false) { // opening on the left
            delay(190);
            Serial.println("LEFT");
            isTurning = true;
            goForwardLeft();
        }
        // Left wall:✅ Right wall:✅
        // Therefore GO STRAIGHT (wall on both sides)
        else if(leftWallDetected && rightWallDetected) {
            if(frontWallDetected) {
            stop();
            finishedFlag = true;
            //turnLeft(2000); // max speed
            //delay(2960);
            }
            else { // going forward and shouldn't stop
            if(isTurning) { // if the robot was in a turn delay a bit to finish the turn successfully
                delay(100);
            }
            isTurning = false;
            if(L_IR_ZONE_DISTANCES[leftIRZone] <= 62){
                goForwardRight();
                delay(40);
            }
            //RIGHT IS CLOSE
            else if(R_IR_ZONE_DISTANCES[rightIRZone] <= 63){
                goForwardLeft();
                delay(40);
            }
            else{
            goForward(200);
            }
            }
        }
    }
    


    if(finishedFlag == true){
        //After enough time passed, look for a detection
        if(elapsedTime > 2960){
            if(leftWallDetected && rightWallDetected && frontWallDetected){
                stop(); 
                delay(99999999);
            }
        }

        goRight(200);

        flash = !flash;
        digitalWrite(C_RED_LED_PIN, flash);
        digitalWrite(L_RED_LED_PIN, flash);
        digitalWrite(R_RED_LED_PIN, flash);
        delay(50);
        elapsedTime += 50;

    }
}




/*
Direction logic




speed is a value added to the 1500 offset in the correct direction
*/
void goForward(int speed) {
    servoLeft.writeMicroseconds(L_SERVO_STALL_PWM + speed);
    // RECALIBRATE to avoid drift, use constant!!!
    servoRight.writeMicroseconds(R_SERVO_STALL_PWM - speed - 20);
}
void goBackward(int speed) {
    servoLeft.writeMicroseconds(L_SERVO_STALL_PWM - speed);
    servoRight.writeMicroseconds(R_SERVO_STALL_PWM + speed);
}
void goLeft(int speed) {
    servoLeft.writeMicroseconds(L_SERVO_STALL_PWM - speed);
    servoRight.writeMicroseconds(R_SERVO_STALL_PWM - speed);
}
void goRight(int speed) {
    servoLeft.writeMicroseconds(L_SERVO_STALL_PWM + speed);
    servoRight.writeMicroseconds(R_SERVO_STALL_PWM + speed);
}




void goForwardLeft() {
    servoLeft.writeMicroseconds(L_SERVO_STALL_PWM + 23); // left go slower
    servoRight.writeMicroseconds(R_SERVO_STALL_PWM - 500); // right go faster
}




void goForwardRight() {
    servoLeft.writeMicroseconds(L_SERVO_STALL_PWM + 500); // left go slower
    servoRight.writeMicroseconds(R_SERVO_STALL_PWM - 23); // right go faster
}


void stop() {
    servoLeft.writeMicroseconds(L_SERVO_STALL_PWM);
    servoRight.writeMicroseconds(R_SERVO_STALL_PWM);
}


/*
Read ir data for a given sensor and receiver at a given frequency




Returns a ZERO if something is detected and a ONE if nothing is detected
*/
int irDetect(int irLedPin, int irReceivePin, long freq)
{
    tone(irLedPin, freq);
    delay(1);
    noTone(irLedPin);
    int ir = digitalRead(irReceivePin);
    delay(1);
    return ir; // 0 is a detection: 1 is no detection
}




int getWallZone(int irReceivePin, int irTransmitPin)
{
    int zoneCount = 0;
    // Iterate over frequencies from 38,000 to 42,000
    // going up by 1000 each time
    for (long f = 38000; f <= 42000; f += 1000)
    {
        zoneCount += irDetect(irTransmitPin, irReceivePin, f);
    }
    return zoneCount;
}

