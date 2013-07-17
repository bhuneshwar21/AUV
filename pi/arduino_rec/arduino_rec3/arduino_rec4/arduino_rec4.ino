// Sweep
// by BARRAGAN <http://barraganstudio.com> 
// This example code is in the public domain.


#include <Servo.h> 
 
Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 
 
int pos = 0;    // variable to store the servo position 
unsigned int buf[7]; 
unsigned char a,b;
unsigned char tempx,tempy,temps,tempe;
int x, y;
double length,length_pre,anglerad,angle,servo_ang;
int check;
void setup() 
{ 
  Serial.begin(57600);
  
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
} 
 
double getangle(void)
{  double tempangle;
  while(Serial.available()){
    a=Serial.read();
    delay(1);
    tempx=Serial.read();
    b=Serial.read();
    tempy=Serial.read(); 
    //Serial.print(a);  Serial.print(" "); Serial.print(tempx);Serial.print(" ");Serial.print(b);Serial.print(" ");Serial.println(tempy);
    if (a ==88 && b==89){
    x=tempx;y=tempy;
    tempangle=atan2(y-60,x-80)*180/3.1415;
    length=sqrt((y-60)*(y-60)+(x-80)*(x-80));
    return tempangle;
    }
  }return -255;
}
//void tunecam(double angle)
//{  double cam_ang;
//   cam_ang=myservo.read();
////  for(pos = 50; pos < 110; pos += 1)  // goes from 0 degrees to 180 degrees 
////  {                                  // in steps of 1 degree 
////    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
////    delay(50);                       // waits 15ms for the servo to reach the position 
////  } 
//  if (length>length_pre)
//    myservo.write(cam_ang+1);
//    length_pre=length;
//    getangle;
//    if (length>length_pre)
//    myservo.write(cam_ang-2);
//    else
//    myservo.write(cam_ang+1);
//}

void loop()
{   //length_pre=length
    double e=0;
    int i=0;
    angle=getangle();
    if (angle!= -255){
    Serial.println(angle);
    servo_ang=myservo.read();
    e=90-abs(angle);
    myservo.write(servo_ang-e/50);
//    if (abs(angle)>90)
//     myservo.write(servo_ang+0.1);
//     else./
//     myservo.write(servo_ang-0.1);
    // delay(5);
    angle=-255; 
    }

    //myservo.write((int)angle);
    delay(10);
}

