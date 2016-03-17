/**The MIT License (MIT)

Copyright (c) 2016 by Seokjin Seo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

See more at http://usemodj.com
*/

// include libraries
#include <Servo.h>
#include "DCMotor.h"
 
// setup servo
#define SERVORIGHT   50
#define SERVOCENTRE 100
#define SERVOLEFT   150
#define SERVOPIN     5
Servo servo;
 
// setup Ultrasonic sensor
#define TRIGPIN 4
#define ECHOPIN 0
 
// setup dc motors - en1, m1-i1, m1-i2, m1-speed, en2, m2-i1, m2-i2, m2-speed
#define DC_SPEED 1023 //255
#define DC_REN 12   //Right Enable
#define DC_R1A 14
#define DC_R2A 16
#define DC_LEN 13   //Left Enable
#define DC_L3A 2
#define DC_L4A 15
DCMotor motors(DC_LEN,DC_L3A,DC_L4A,DC_SPEED, DC_REN,DC_R1A,DC_R2A,DC_SPEED);

int stopCount = 0;

int ping()
{
    // pause for 50ms between scans
    delay(50);
 
    // send ping
    digitalWrite(TRIGPIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGPIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGPIN, LOW);
 
    // read echo
    long duration = pulseIn(ECHOPIN, HIGH);
 
    // convert distance to cm
    unsigned int centimetres = int(duration / 2 / 29.1);
 
    return centimetres;
}
 
char scan()
{
    // ping times in microseconds
    unsigned int left_scan, centre_scan, right_scan;
    char choice;
 
    // scan left
    servo.write(SERVOLEFT);
    delay(300);
    left_scan = ping();
 
    // scan right
    servo.write(SERVORIGHT);
    delay(600);
    right_scan = ping();
 
    // scan straight ahead
    servo.write(SERVOCENTRE);
    delay(300);
    centre_scan = ping();
 
    if (left_scan>right_scan && left_scan>centre_scan)
    {
        choice = 'l';
    }
    else if (right_scan>left_scan && right_scan>centre_scan)
    {
        choice = 'r';
    }
    else {
      choice = 'c';
    }
 
    return choice;
}
 
void setup()
{
    Serial.begin(115200);
    Serial.println("MakrFun Obstacle Bot 2WD v1.0");
    // set the servo data pin
    servo.attach(SERVOPIN);
 
    // set the trig pin to output (send sound waves)
    pinMode(TRIGPIN, OUTPUT);
 
    // set the echo pin to input (receive sound waves)
    pinMode(ECHOPIN, INPUT);

}

void loop()
{
    // get distance from obstacle straight ahead
    unsigned int distance = ping();
    Serial.print("Distance: "); Serial.println(distance);
    
    if (distance < 50 && distance > 0)
    {
        if (distance < 10)
        {
            // turn around
            Serial.println("Turn around...");            
            motors.backward();
            delay(300);
            motors.left();
            delay(500);
        }
        else
        {
            // stop both motors
            Serial.println("Motor stop...");
            motors.stop();
            
            // scan for obstacles
            char turn_direction = scan();
 
            // turn left/right or ignore and go straight
            if (turn_direction == 'l')
            {
              Serial.println("Turn left...");
                motors.left();
                delay(200);
            }
            else if (turn_direction == 'r')
            {
              Serial.println("Turn right...");
                motors.right();
                delay(200);
            }
            else if (turn_direction == 'c')
            {
              stopCount++;
              if(stopCount > 3){
                stopCount = 0;
                Serial.println("Turn back...");
                motors.right();
                delay(400);
              }
              
            }
        }
    }
    else
    {
        // no obstacle, keep going forward
        Serial.println("No obstacle, keep going forward...");
        motors.forward();
    }
}


