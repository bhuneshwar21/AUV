// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
 
void setup() 
{ 
  Serial.begin(57600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
} 
 
 
void loop()
{ 
  unsigned int temp;
  temp=Serial.read();
  Serial.println(temp);
//  for(pos = 0; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
//  {                                  // in steps of 1 degree 
//    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
//    delay(15);                       // waits 15ms for the servo to reach the position 
//  } 
//  for(pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
//  {                                
//    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
//    delay(15);                       // waits 15ms for the servo to reach the position 
//  } 
//  while (Serial.available())
//  {
//    unsigned int temp,temp1,temp2; 
//    double value;
//    unsigned int inChar = Serial.read();
//    if (inChar == 254)
//    {
//      Serial.print('y');
//      temp=Serial.read();
//      Serial.print(temp);
//      Serial.print(' ');
//      temp1=Serial.read();
//      Serial.print(temp1);
//      Serial.print(' ');
//      temp2=Serial.read();
//      //Serial.println(temp2);
//      value=temp+temp1*256;
//      Serial.println(value);
//      Serial.print('z');
//      Serial.print(temp2);
//      Serial.print(' ');
//      if (temp2==255)
//      {    
//        if (value > 32768)
//        {
//          value = -(value -32768);
//        }
//        value=(double)value/100;
//
//      Serial.println(value);
//     }
//  }
//} 
}
