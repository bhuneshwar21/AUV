byte  speed_l_temp, speed_r_temp, speed_ud_temp;
byte  speed_l, speed_r, speed_ud;
byte  check1, check2;
int target_depth = 130;
double error;

void setup()
{
  Serial.begin(57600);
  Serial.flush();
}

void get_speed()
{
  while(Serial.available()==0);
  check1 = Serial.read();
  if(check1 == 253)
  {
    delay(1);
    speed_l_temp = Serial.read(); delay(1);
    speed_r_temp = Serial.read(); delay(1);
    check2 = Serial.read();
    if(check2 == (speed_l_temp + speed_r_temp)%256)
    {
      speed_l = speed_l_temp;
      speed_r = speed_r_temp;
      Serial.print(speed_l);Serial.print(" ");Serial.println(speed_r);
      analogWrite(2, speed_l);
      analogWrite(3, 125);
      analogWrite(4, speed_r);
      analogWrite(5, 125);
      Serial.flush();
    }
  }
}
void loop()
{
  if(analogRead(2)>90) 
  {
//    error = (analogRead(2)-target_depth);
//    if(error>3 || error<-3)
//    {
//      speed_ud = 125 + error*10;
//      analogWrite(10, speed_ud);
//      analogWrite(11, 125);
//      analogWrite(8, speed_ud);
//      analogWrite(9, 125);
//    }
      analogWrite(10, 0);
      analogWrite(11, 100);
      analogWrite(8, 0);
      analogWrite(9, 80);

    get_speed();
    
  }
  else
  {
    analogWrite(2,0);
    analogWrite(3,0);
    analogWrite(4,0);
    analogWrite(5,0);
    analogWrite(10,0);
    analogWrite(11,0);
    analogWrite(8,0);
    analogWrite(9,0);
  }
    
}

