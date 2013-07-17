byte check1;
byte speed_l,speed_r;
byte check2;

void setup()
{
 Serial.begin(57600);
 Serial.flush();
}

void loop()
{
  while(Serial.available())
  {
    if(Serial.read()==253){
   delay(1);
   speed_l=Serial.read();delay(1);
   speed_r=Serial.read();delay(1);
   check2=Serial.read();delay(1);
   if ((speed_l+speed_r)%256==check2)
   {
   Serial.print(speed_l-125);Serial.print(" ");Serial.println(speed_r-125);
   }}
  }


}
