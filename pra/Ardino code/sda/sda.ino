#include <Servo.h>

 

#define MIN_Y 0

#define MAX_Y 180

 

#define MIN_X 0

#define MAX_X 180

 

Servo tilt;

Servo pan;

 

int curX = MAX_X/2;

int curY = MAX_Y/2;
int x,y,c;

 

void setup() {

Serial.begin( 57600 );

pan.attach( 11 );

tilt.attach( 12 );

tilt.write( curX );

pan.write( curY );

}

 

void loop() {

while( Serial.available()){
c=0;
c = Serial.read();

x = ( c % 10 ) - 2;

y = ( c / 10 ) - 2;

curX += 2*x;

curY += 2*y;

if( curX > MAX_X )

curX = MAX_X;

if( curY > MAX_Y )

curY = MAX_Y;

if( curX < MIN_X )

curX = MIN_X;

if( curY < MIN_Y )

curY = MIN_Y;

tilt.write( curX );

pan.write( curY );
}


//if(Serial.available() == 0){
//  for(int pos =0; pos < 180; pos +=1){
//    pan.write(pos);
//    delay(15);
//  }
//  for(int pos = 180; pos>=1; pos-=1)     // goes from 180 degrees to 0 degrees 
//  {                                
//    pan.write(pos);              // tell servo to go to position in variable 'pos' 
//    delay(15);                       // waits 15ms for the servo to reach the position 
//  } 
//}
}
  
